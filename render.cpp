#include "game.h"
#include "textures.h"
#include "collision.h"
#include "render.h"
#include "isometric_calc.h"

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <unordered_set>

RenderQueueItem::RenderQueueItem(int z_index, SDL_Rect dstrect, Texture* texture) {
    this->z_index = z_index;
    this->dstrect = dstrect;
    this->texture = texture;
}
RenderQueueItem::RenderQueueItem(int z_index, std::function<void(SDL_Renderer* renderer)> custom_render) {
    this->z_index = z_index;
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

std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets;
static const std::unordered_set<int> ground_values = { 1, 2, 8 };

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player) {
    // render q's ei ole groundi pildid ehk id 1, 4, 5. V6ib lisanduda!
    std::vector<RenderQueueItem> render_queue;
    int render_diameter = render_radius * 2;
    render_queue.reserve(render_diameter * render_diameter);

    int player_tile_x = static_cast<int>(player.x / tile_size);
    int player_tile_y = static_cast<int>(player.y / tile_size);

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

            SDL_FPoint isometric_coordinates = to_isometric_grid_coordinate(offset, column, row);
            int row_coord = isometric_coordinates.x;
            int col_coord = isometric_coordinates.y;
            SDL_Rect destTile = { row_coord, col_coord, tile_size, tile_size };

            // Searching for grid_value in ground_values
            if (ground_values.find(grid_value) != ground_values.end()) {
                texture_map[Map::GROUND_CUBE].render(renderer, &destTile);
            }


            switch (grid_value)
            {
                // simple textures that can be immediately rendered
            case Map::MAZE_GROUND_VAL:
            case Map::SNOWY_GROUND_VAL:
            case Map::YELLOW_VAL:
            case Map::ERROR_VAL:
            case Map::BLUE_VAL: {
                texture_map[grid_value].render(renderer, &destTile);
                break;
            }

            case Map::TREE_VAL: {
                destTile.y -= half_tile;
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[grid_value])
                );
                break;
            }
                              // these tree use the same texture atm
            case Map::WALL_VAL:
            case Map::SECTOR_1_WALL_VAL:
            case Map::SECTOR_3_WALL_VAL: {
                destTile.y -= half_tile;
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[Map::WALL_VAL])
                );
                break;
            }
            case Map::INGROWN_WALL_VAL: {
                destTile.y -= half_tile;
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[Map::INGROWN_WALL_VAL])
                );
                break;
            }
            case Map::WALL_VAL_VINE: case Map::SECTOR_2_WALL_VAL: {
                texture_map[Map::GROUND_CUBE].render(renderer, &destTile);
                // Only generate random offset once per block
                // see 20 on kui palju px on maxist v2hem
                int xr = random_offsets.try_emplace(grid_pos, rand() % 20)
                    .first->second;

                destTile.y -= half_tile - (xr / 2);
                destTile.x += (xr / 2);
                destTile.w -= xr;
                destTile.h -= xr;

                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, &texture_map[Map::WALL_VAL])
                );

                destTile.y += 1;  // +1 sest muidu hakkab walli destTile'iga v6itlema ja flickerib.
                auto cube_vine_tex = choose_cube_vine_texture("", grid_pos);
                render_queue.push_back(
                    RenderQueueItem(destTile.y, destTile, cube_vine_tex)
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
                SDL_RenderDrawRect(renderer, &destTile);
                // TODO: make the isometric projection of player collision box
                // just for debugging purposes, SDL_RenderDrawPoints might be useful
            }
        }
    }

    // NOTE: KUI VARA PLAYER V2LJA TULEB  - half_tile
        // tuleks teha eraldi igale objektile, kus on see l2vend, et player on TOP
    SDL_Rect player_int_rect = { static_cast<int>(player.rect.x),
                                 static_cast<int>(player.rect.y),
                                 static_cast<int>(player.rect.w),
                                 static_cast<int>(player.rect.h)
    };
    render_queue.push_back(
        RenderQueueItem(player_int_rect.y, [&offset](SDL_Renderer* renderer) { draw_player(renderer, offset); })
    );

    std::sort(render_queue.begin(), render_queue.end(), [](const RenderQueueItem& a, const RenderQueueItem& b) {
        return a.z_index < b.z_index;
        });

    // render things in queue
    for (auto& r : render_queue) {
        r.render(renderer);
    }
    render_queue.clear();
}


void render_map_numbers(SDL_Renderer* renderer, struct Offset& offset, struct Player& player)
{
    int player_tile_y = player.y / tile_size;
    int bottom = player_tile_y + render_radius;
    int top = player_tile_y - render_radius;

    int player_tile_x = player.x / tile_size;
    int left = player_tile_x - render_radius;
    int right = player_tile_x + render_radius;

    int half_tile = tile_size / 2;

    for (int y = 0; y < map_size; y++) {
        if (y < top || y > bottom) continue;

        for (int x = 0; x < map_size; x++) {
            if (x < left || x > right) continue;

            SDL_FPoint isometric_coordinates = to_isometric_grid_coordinate(offset, x, y);
            int row_coord = isometric_coordinates.x;
            int col_coord = isometric_coordinates.y;
            SDL_Rect destTile = { row_coord + (tile_size / 4), col_coord, half_tile, half_tile };

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