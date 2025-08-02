#include "game.hpp"
#include "textures.hpp"
#include "collision.hpp"
#include "render.hpp"
#include "isometric_calc.hpp"
#include "raycast.hpp"

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
RenderQueueItem::RenderQueueItem(int render_order, std::function<void(SDL_Renderer* renderer)> custom_render) {
    this->render_order = render_order;
    this->custom_render = custom_render;
}

void RenderQueueItem::render(SDL_Renderer* renderer) {
    if (this->texture.has_value()) {
        return this->texture.value()->render(renderer, &this->dstrect);
    }
    if (this->custom_render.has_value()) {
        return this->custom_render.value()(renderer);
    }

    throw std::logic_error("texture or render function needed for rendering");
}

std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets_walls;
std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets_trees;
std::set<std::pair<int, int>> grid_vine_checked;

static const std::unordered_set<int> ground_values = {
    Map::GROUND_CUBE, Map::TREE, Map::GRASS_COVER, Map::TREE_TRUNK
};

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
            std::pair<int, int> grid_pos = { column, row };

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

            // Searching for grid_value in ground_values
            if (ground_values.find(grid_value) != ground_values.end()) {
                texture_map[Map::GROUND_CUBE].render(renderer, &destTile);
            }
            // Maze pathway texture under walls
            if (wall_values.find(grid_value) != wall_values.end() ||
                grid_value == Map::VINE_OVERHANG_SN ||
                grid_value == Map::VINE_OVERHANG_EW) {
                texture_map[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
            }

            switch (grid_value) {
            case Map::VOID_CUBE:
            case Map::VOID_CUBE_NEIGHBOUR: {
                render_void_tilemap(renderer, offset, map, grid_pos, destTile);
                break;
            }
                                         // simple textures that can be immediately rendered (no render_q)
            case Map::MAZE_GROUND_CUBE:
            case Map::YELLOW_CUBE:
            case Map::ERROR_CUBE:
            case Map::LAND_CUBE:
            case Map::BLUE_CUBE: {
                texture_map[grid_value].render(renderer, &destTile);
                break;
            }
            case Map::TREE_TRUNK: {
                int xr = random_offsets_trees.try_emplace(grid_pos, rand() % 20)
                    .first->second;
                destTile.x += (tile_size / 5) + xr;
                destTile.y += (tile_size / 3) + xr;
                destTile.h = (tile_size / 2) - xr;
                destTile.w = (tile_size / 2) - xr;
            }
                                // textures above ground, 1st floor (no render_q)
            case Map::GRASS_COVER: {
                destTile.y -= half_tile;
                texture_map[grid_value].render(renderer, &destTile);
                break;
            }
            case Map::TREE: {
                int xr = random_offsets_trees.try_emplace(grid_pos, rand() % 125)
                    .first->second;
                destTile.y -= half_tile - (xr / 5);
                destTile.x += (xr / 8);
                destTile.w -= (xr / 4);
                destTile.h -= (xr / 4);
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[grid_value])
                );
                break;
            }
                          // textures above ground i.e .y -= half_tile
            case Map::VINE_OVERHANG_SN:
            case Map::VINE_OVERHANG_EW:
            case Map::VINE_COVER_N: {
                destTile.y -= half_tile;
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[grid_value])
                );
                break;
            }
            case Map::SECTOR_3_WALL_VAL: {
                destTile.y -= half_tile;
                if (wall_values.find(map[row + 1][column]) != wall_values.end() &&
                    wall_values.find(map[row][column + 1]) != wall_values.end() &&
                    wall_values.find(map[row - 1][column]) != wall_values.end() &&
                    wall_values.find(map[row][column - 1]) != wall_values.end()) {
                    render_queue.push_back(
                        RenderQueueItem(destTile.y, destTile, &texture_map[Map::WALL_CUBE])
                    );
                    break;
                }
                // random block size code code
                // destTile.y += (tile_size / 9);
                // destTile.x += (tile_size / 16);
                destTile.w -= (tile_size / 8);
                // destTile.h -= (tile_size / 8);
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[Map::WALL_CUBE])
                );
                break;
            }
                                       // these tree use the same texture atm
            case Map::WALL_CUBE:
            case Map::SECTOR_1_WALL_VAL: {
                destTile.y -= half_tile;
                // random block size code code
                destTile.y += (tile_size / 9);
                destTile.x += (tile_size / 16);
                destTile.w -= (tile_size / 8);
                destTile.h -= (tile_size / 8);
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[Map::WALL_CUBE])
                );
                break;
            }
                                       // section wall, thicker wall
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
            case Map::VINE_WALL:
            case Map::SECTOR_2_WALL_VAL: {
                // vaata grid_below, sest Vine tekstuur on vaid NS orientatsiooniga
                int grid_below = map[row - 1][column];
                if (rand() % 4 == 1 && !grid_vine_checked.count(grid_pos)) {
                    // add overhang vines VINE_OVERHANG_SN
                    if (grid_below == Map::MAZE_GROUND_CUBE &&
                        (map[row - 2][column] == Map::VINE_WALL ||
                            map[row - 2][column] == Map::SECTOR_2_WALL_VAL)) {
                        map[row - 1][column] = Map::VINE_OVERHANG_SN;
                    }
                    // add overhang vines VINE_OVERHANG_EW
                    if (map[row][column + 1] == Map::MAZE_GROUND_CUBE &&
                        (map[row][column + 2] == Map::VINE_WALL ||
                            map[row][column + 2] == Map::SECTOR_2_WALL_VAL)) {
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
                // Only generate random offset once per block
                // see rand % number on kui palju px on maxist v2hem
                int xr = random_offsets_walls.try_emplace(grid_pos, rand() % 40)
                    .first->second;
                destTile.y -= half_tile - (xr / 2.5);
                destTile.x += (xr / 4);
                destTile.w -= (xr / 2);
                destTile.h -= (xr / 2);
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[Map::WALL_CUBE])
                );
                // vine'il y += 1, et vine tekstuur oleks on top of wall
                auto cube_vine_tex = choose_cube_vine_texture("", grid_pos);
                render_queue.push_back(
                    RenderQueueItem(destTile.y + 1, destTile, cube_vine_tex)
                );
                break;
            }

            default:
                break;
            }

            // Player tile highlight (render last)
            if (row == player_tile_y && column == player_tile_x) {
                destTile.h = half_tile;
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderRect(renderer, &destTile);
                // TODO: make the isometric projection of player collision box
                // just for debugging purposes, SDL_RenderDrawPoints might be useful
            }
        }
    }
    float render_after = -(tile_size / 2);
    render_queue.push_back(
        RenderQueueItem(static_cast<int>(player.rect.y + render_after), [](SDL_Renderer* renderer) { load_player_sprite(renderer); })
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

void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player)
{
    /* Vali ise, mis mappi tahad geneda. */
    render_map(renderer, offset, player);
    // render_map_numbers(renderer, offset, player);
}