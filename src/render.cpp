#include "game.hpp"
#include "textures.hpp"
#include "collision.hpp"
#include "render.hpp"
#include "isometric_calc.hpp"
#include "raycast.hpp"
#include "mod_map_data.hpp"
#include "animation.hpp"

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <unordered_set>
#include <utility>
#include <set>

RenderQueueItem::RenderQueueItem(int render_order, SDL_FRect dstrect, Texture* texture) {
    this->render_order = render_order;
    this->dstrect = dstrect;
    this->texture = texture;
}
RenderQueueItem::RenderQueueItem(int render_order, SDL_FRect srcrect, SDL_FRect dstrect, Texture* texture) {
    this->render_order = render_order;
    this->dstrect = dstrect;
    this->srcrect = srcrect;
    this->texture = texture;
}
RenderQueueItem::RenderQueueItem(int render_order, std::function<void(SDL_Renderer* renderer)> custom_render) {
    this->render_order = render_order;
    this->custom_render = custom_render;
}

void RenderQueueItem::render(SDL_Renderer* renderer) {
    if (this->texture.has_value()) {
        if (srcrect.h != 0 && srcrect.w != 0) {
            return this->texture.value()->render(renderer, &this->srcrect, &this->dstrect);
        }
        else {
            return this->texture.value()->render(renderer, &this->dstrect);
        }
    }
    if (this->custom_render.has_value()) {
        return this->custom_render.value()(renderer);
    }

    throw std::logic_error("texture or render function needed for rendering");
}

std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets_walls;
std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets_trees;
std::unordered_map<std::pair<int, int>, int, pair_hash> grassCoverGrids;
std::unordered_map<std::pair<int, int>, int, pair_hash> mazeDecoMap;
std::unordered_map<std::pair<int, int>, int, pair_hash> mazeGroundMap;
std::set<std::pair<int, int>> grid_vine_checked;

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player) {
    // render q's ei ole groundi pildid ehk id 1, 4, 5. V6ib lisanduda!
    std::vector<RenderQueueItem> render_queue;
    int render_diameter = render_radius * 2;
    render_queue.reserve(render_diameter * render_diameter);

    int player_tile_x = static_cast<int>((player.x + (player.size / 2)) / tile_size);
    int player_tile_y = static_cast<int>((player.y + (player.size / 2)) / tile_size);

    int left = player_tile_x - render_radius;
    int right = player_tile_x + render_radius;
    int top = player_tile_y - render_radius;
    int bottom = player_tile_y + render_radius;

    int half_tile = tile_size / 2;

    for (int row = 0; row < map_size; row++) {
        if (row < top || row > bottom) continue;

        for (int column = 0; column < map_size; column++) {
            if (column < left || column > right) continue;

            int grid_value = map[row][column];
            std::pair<int, int> grid_pos = { row, column };

            if (r_pressed == false) {
                // pass through
            }
            //  if gridPos not inside endpointActiveGrids
            else if (Raycast::endpointActiveGrids.find(grid_pos) == Raycast::endpointActiveGrids.end()) {
                // continue if gridPos inside decayGrids
                if (std::find(Raycast::decayGrids.begin(), Raycast::decayGrids.end(), grid_pos)
                    == Raycast::decayGrids.end()) {
                    continue;
                }
            }

            SDL_FPoint isometric_coordinates = to_isometric_grid_coordinate(offset, column, row);
            float row_coord = isometric_coordinates.x;
            float col_coord = isometric_coordinates.y;
            SDL_FRect destTile = { row_coord, col_coord, tile_size, tile_size };
            SDL_FRect srcFRect;
            // Player tile highlight (render last)
            if (row == player_tile_y && column == player_tile_x) {
                render_queue.push_back(
                    RenderQueueItem(destTile.y - 1, destTile, &texture_map[Map::INVISIBLE_CUBE])
                );
            }
            // GRID_VALUES IN GROUND_VALUES
            if (ground_values.find(grid_value) != ground_values.end()) {
                texture_map[Map::GROUND_CUBE].render(renderer, &destTile);
            }
            if (wall_values.find(grid_value) != wall_values.end()) {
                texture_map[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
            }
            if (grid_value == Map::VINE_OVERHANG_SN || grid_value == Map::VINE_OVERHANG_EW) {
                srcFRect = return_src_1x3(grid_pos, mazeGroundMap);
                texture_map[Map::MAZE_GROUND_SPRITE].render(renderer, &srcFRect, &destTile);
            }
            switch (grid_value) {
                case Map::VOID_CUBE:
                case Map::VOID_CUBE_NEIGHBOUR: {
                    render_void_tilemap(renderer, offset, map, grid_pos, destTile);
                    break;
                }
                                            // simple textures that can be immediately rendered (no render_q)
                case Map::MAZE_GROUND_CUBE: {
                    srcFRect = return_src_1x3(grid_pos, mazeGroundMap);
                    if (isEmpty(srcFRect)) texture_map[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
                    else texture_map[Map::MAZE_GROUND_SPRITE].render(renderer, &srcFRect, &destTile);
                    srcFRect = return_src_1x3(grid_pos, mazeDecoMap);
                    if (isEmpty(srcFRect)) break;
                    destTile.y -= half_tile;
                    texture_map[Map::MAZE_DECO].render(renderer, &srcFRect, &destTile);
                    break;
                }
                case Map::YELLOW_CUBE:
                case Map::ERROR_CUBE:
                case Map::LAND_CUBE:
                case Map::BLUE_CUBE: {
                    texture_map[grid_value].render(renderer, &destTile);
                    break;
                }
                case Map::TREE_TRUNK: {
                    destTile.y -= half_tile;
                    int xr = random_offsets_trees.try_emplace(grid_pos, rand() % 20)
                        .first->second;
                    SDL_FRect tempTile = destTile;
                    tempTile.x += (tile_size / 5) + xr;
                    tempTile.y += (tile_size / 3) + xr;
                    tempTile.h = (tile_size / 2) - xr;
                    tempTile.w = (tile_size / 2) - xr;
                    texture_map[grid_value].render(renderer, &tempTile);
                    int grassIndex = create_random_grass(grid_pos, grid_value);
                    if (grassIndex != -1) texture_map[grassIndex].render(renderer, &destTile);
                    break;
                }
                                    // textures above ground, 1st floor (no render_q)
                case Map::GRASS_COVER: {
                    destTile.y -= half_tile;
                    int varIndex = grassCoverGrids.try_emplace(grid_pos, rand() % 3)
                        .first->second;
                    int grassIndex = create_random_grass(grid_pos, grid_value);
                    if (grassIndex != -1) texture_map[grassIndex].render(renderer, &destTile);
                    break;
                }
                case Map::TREE: {
                    destTile.y -= half_tile;
                    int xr = random_offsets_trees.try_emplace(grid_pos, rand() % 125)
                        .first->second;
                    SDL_FRect tempTile = destTile;
                    tempTile.y += (xr / 5);
                    tempTile.x += (xr / 8);
                    tempTile.w -= (xr / 4);
                    tempTile.h -= (xr / 4);
                    render_queue.push_back(
                        RenderQueueItem(tempTile.y, tempTile, &texture_map[grid_value])
                    );
                    int grassIndex = create_random_grass(grid_pos, grid_value);
                    if (grassIndex != -1) texture_map[grassIndex].render(renderer, &destTile);
                    break;
                }
                            // textures above ground i.e .y -= half_tile
                case Map::VINE_COVER_N:
                case Map::VINE_OVERHANG_SN:
                case Map::VINE_OVERHANG_EW: {
                    texture_map[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
                    destTile.y -= half_tile;
                    render_queue.push_back(
                        RenderQueueItem(destTile.y, destTile, &texture_map[grid_value])
                    );
                    break;
                }
                                        // section seperating wall, thicker wall
                case Map::INGROWN_WALL_CUBE: {
                    destTile.y -= half_tile;
                    destTile.y -= 5;
                    destTile.x -= 5;
                    destTile.w += 5;
                    destTile.h += 5;
                    render_queue.push_back(
                        RenderQueueItem(destTile.y, destTile, &texture_map[grid_value])
                    );
                    destTile.y -= half_tile;
                    render_queue.push_back(
                        RenderQueueItem(destTile.y + half_tile + 1, destTile, &texture_map[Map::WALL_CUBE])
                    );
                    break;
                }
                case Map::WALL_CUBE:
                case Map::SECTOR_1_WALL_VAL:
                case Map::SECTOR_2_WALL_VAL:
                case Map::SECTOR_3_WALL_VAL: {
                    destTile.y -= half_tile;
                    if (grid_value == Map::SECTOR_3_WALL_VAL
                        && unchangable_walls_s3.find(grid_pos) == unchangable_walls_s3.end()) return;
                    srcFRect = return_src_1x3(grid_pos, mazeGroundMap);
                    if (grid_value == Map::SECTOR_2_WALL_VAL) {
                        int xr = random_offsets_walls.try_emplace(grid_pos, rand() % 30)
                            .first->second;
                        destTile.y += (xr / 2.5);
                        destTile.x += (xr / 4);
                        destTile.w -= (xr / 2);
                        destTile.h -= (xr / 2);
                    }
                    if (!isEmpty(srcFRect)) {
                        render_queue.push_back(
                            RenderQueueItem(destTile.y, srcFRect, destTile, &texture_map[Map::WALL_CUBE_SPRITE])
                        );
                    }
                    else {
                        render_queue.push_back(
                            RenderQueueItem(destTile.y, srcFRect, destTile, &texture_map[Map::WALL_CUBE])
                        );
                    }
                    // create vines on top of SECTOR_2 walls
                    if (grid_value == Map::SECTOR_2_WALL_VAL) {
                        // vaata grid_below, sest Vine tekstuur on vaid NS orientatsiooniga
                        int grid_below = map[row - 1][column];
                        if (rand() % 4 == 1 && !grid_vine_checked.count(grid_pos)) {
                            // add overhang vines VINE_OVERHANG_SN
                            if (grid_below == Map::MAZE_GROUND_CUBE &&
                                (map[row - 2][column] == Map::SECTOR_2_WALL_VAL)) {
                                map[row - 1][column] = Map::VINE_OVERHANG_SN;
                            }
                            // add overhang vines VINE_OVERHANG_EW
                            if (map[row][column + 1] == Map::MAZE_GROUND_CUBE &&
                                (map[row][column + 2] == Map::SECTOR_2_WALL_VAL)) {
                                map[row][column + 1] = Map::VINE_OVERHANG_EW;
                            }
                        }
                        if (rand() % 10 == 1 && !grid_vine_checked.count(grid_pos)) {
                            // add VINE_COVER_N to wall's southern side
                            if (grid_below == Map::MAZE_GROUND_CUBE ||
                                grid_below == Map::VINE_OVERHANG_SN ||
                                grid_below == Map::VINE_OVERHANG_EW) {
                                map[row - 1][column] = Map::VINE_COVER_N;
                                // expand vine to neighbouring blocks aswell.
                                // FIXME: siin on bug. Kui vine on 6hus ss see ie ole feature. hetkel mitte.
                                // Should be recursion but maze direction doesn't allow longer than 3 walls in sector2
                                if (map[row][column + 1] == Map::VINE_OVERHANG_SN ||
                                    map[row][column + 1] == Map::VINE_OVERHANG_EW) {
                                    map[row - 1][column + 1] = Map::VINE_COVER_N;
                                }
                                if (map[row][column - 1] == Map::VINE_OVERHANG_SN ||
                                    map[row][column - 1] == Map::VINE_OVERHANG_EW) {
                                    map[row - 1][column - 1] = Map::VINE_COVER_N;
                                }
                            }
                        }
                        grid_vine_checked.insert(grid_pos);
                        // vine'il y += 1, et vine tekstuur oleks on top of wall
                        auto cube_vine_tex = choose_cube_vine_texture("", grid_pos);
                        if (cube_vine_tex == nullptr) break;
                        render_queue.push_back(
                            RenderQueueItem(destTile.y + 1, destTile, cube_vine_tex)
                        );
                        break;
                    }
                    // create maze deco into sector 3
                    else if (grid_value == Map::SECTOR_3_WALL_VAL) {
                        srcFRect = return_src_1x3(grid_pos, mazeDecoMap);
                        if (!isEmpty(srcFRect)) render_queue.push_back(
                            RenderQueueItem(destTile.y + 1, srcFRect, destTile, &texture_map[Map::WALL_MARKINGS])
                        );
                        destTile.y -= half_tile;
                        srcFRect = return_src_1x3(grid_pos, mazeDecoMap);
                        if (isEmpty(srcFRect)) break;
                        render_queue.push_back(
                            RenderQueueItem(destTile.y + half_tile + 1, srcFRect, destTile, &texture_map[Map::MAZE_DECO])
                        );
                        break;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    float render_after = -(tile_size / 2);
    render_queue.push_back(
        RenderQueueItem(static_cast<int>(player.rect.y + render_after), [](SDL_Renderer* renderer) { animation_player(renderer); })
    );
    std::sort(render_queue.begin(), render_queue.end(), [](const RenderQueueItem& a, const RenderQueueItem& b) {
        return a.render_order < b.render_order;
        });
    // render things in queue
    for (auto& r : render_queue) {
        r.render(renderer);
    }
    render_queue.clear();
}


void render_map_numbers(SDL_Renderer* renderer, struct Offset& offset, struct Player& player) {
    int player_tile_y = player.y / tile_size;
    int bottom = player_tile_y + render_radius;
    int top = player_tile_y - render_radius;

    int player_tile_x = player.x / tile_size;
    int left = player_tile_x - render_radius;
    int right = player_tile_x + render_radius;

    float half_tile = tile_size / 2;

    for (int y = 0; y < map_size; y++) {
        if (y < top || y > bottom) continue;

        for (int x = 0; x < map_size; x++) {
            if (x < left || x > right) continue;

            SDL_FPoint isometric_coordinates = to_isometric_grid_coordinate(offset, x, y);
            float row_coord = isometric_coordinates.x;
            float col_coord = isometric_coordinates.y;
            SDL_FRect destTile = { row_coord + (tile_size / 4), col_coord, half_tile, half_tile };
            load_specific_number(renderer, map[y][x], destTile);
        }
    }
}

int create_random_grass(std::pair<int, int> grid_pos, int grid_value) {
    int varIndex = grassCoverGrids.try_emplace(grid_pos, rand() % 4)
        .first->second;
    if (varIndex == 0) {
        return Map::GRASS_COVER_SHORT;
    }
    else if (varIndex == 1) {
        return Map::GRASS_COVER_TALL;
    }
    else if (varIndex == 2) {
        return Map::GRASS_COVER;
    }
    else {
        return -1;
    }
}

SDL_FRect return_src_1x3(std::pair<int, int> grid_pos,
    std::unordered_map<std::pair<int, int>, int, pair_hash>& map) {
    float sprite_width = 32;
    float sprite_height = 32;
    int varIndex = map.try_emplace(grid_pos, rand() % 4)
        .first->second;

    if (varIndex == 0) {
        return SDL_FRect{ 0, 0, sprite_width, sprite_height };
    }
    else if (varIndex == 1) {
        return SDL_FRect{ 32, 0, sprite_width, sprite_height };
    }
    else if (varIndex == 2) {
        return SDL_FRect{ 64, 0, sprite_width, sprite_height };
    }
    else {
        return SDL_FRect{ 0, 0, 0, 0 };
    }
}

void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player)
{
    /* Vali ise, mis mappi tahad geneda. */
    render_map(renderer, offset, player);
    // render_map_numbers(renderer, offset, player);
}