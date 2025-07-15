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

std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets;
static const std::unordered_set<int> ground_values = { 1, 2, 8 };

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player) {

    struct Renderable {
        SDL_Texture* tex = nullptr;
        SDL_Rect dest;
        int sortY;
        std::function<void()> custom_draw = nullptr; // Optional for special draw logic
    };

    std::vector<Renderable> render_queue;  // render q's ei ole groundi pildid ehk id 1, 4, 5. V6ib lisanduda!

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


            SDL_FPoint isometric_coordinates = to_isometric_grid_coordinate(offset, column, row);

            int row_coord = isometric_coordinates.x;
            int col_coord = isometric_coordinates.y;

            SDL_Rect destTile = { row_coord, col_coord, tile_size, tile_size };

            int grid_value = map[row][column];
            std::pair<int, int> grid_pos = { row, column };

            // Searching for grid_value in ground_values
            auto it = ground_values.find(grid_value);
            if (ground_values.find(grid_value) != ground_values.end()) {
                load_cube_ground_texture(renderer, destTile);
            }


            switch (grid_value)
            {
            case 2: { // tree.png
                destTile.y -= half_tile;
                render_queue.push_back(Renderable{ tree_tex, destTile, destTile.y });
                break;
            }
            case 4: { // maze_ground_cube.png
                load_cube_maze_ground_texture(renderer, destTile);
                break;
            }
            case 5: { // snowy_ground_cube.png
                load_cube_snowy_ground_texture(renderer, destTile);
                break;
            }
            case 6: { // yellow_cube.png
                load_cube_yellow_texture(renderer, destTile);
                break;
            }
            case 7: { // error_cube.png
                load_cube_error_texture(renderer, destTile);
                break;
            }
            case 66: { // blue_cube.png
                load_cube_blue_texture(renderer, destTile);
                break;
            }

            case 9: case 91: case 93: { // walls | wall_cube.png
                destTile.y -= half_tile;
                render_queue.push_back(Renderable{ cube_wall_tex, destTile, destTile.y });
                break;
            }
            case 94: { // hairy bottom walls | ingrown_wall_cube.png
                destTile.y -= half_tile;
                render_queue.push_back(Renderable{ cube_ingrown_wall_tex, destTile, destTile.y });
                break;
            }
            case 8: case 92: { // wall cubes with vines
                load_cube_ground_texture(renderer, destTile);
                // Only generate random offset once per block
                if (random_offsets.find(grid_pos) == random_offsets.end()) {
                    random_offsets[grid_pos] = rand() % 20;  // see 20 on kui palju px on maxist v2hem
                }
                int xr = random_offsets[grid_pos];

                destTile.y -= half_tile - (xr / 2);
                destTile.x += (xr / 2);
                destTile.w -= xr;
                destTile.h -= xr;

                render_queue.push_back(Renderable{ cube_wall_tex, destTile, destTile.y });

                destTile.y += 1;  // +1 sest muidu hakkab walli destTile'iga v6itlema ja flickerib.
                auto cube_vine_tex = choose_cube_vine_texture("", grid_pos);
                render_queue.push_back(Renderable{ cube_vine_tex, destTile, destTile.y });
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
    render_queue.push_back(Renderable{ nullptr, player_int_rect, player_int_rect.y, [=]() { load_player_sprite(renderer); } });

    // Sort
    std::sort(render_queue.begin(), render_queue.end(), [](const Renderable& a, const Renderable& b) {
        return a.sortY < b.sortY;
        });

    // Render all
    for (auto& r : render_queue) {
        if (r.custom_draw) {
            r.custom_draw();
        }
        else if (r.tex) {
            SDL_RenderCopy(renderer, r.tex, nullptr, &r.dest);
        }
    }
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