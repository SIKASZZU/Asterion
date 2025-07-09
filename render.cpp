#include "game.h"
#include "textures.h"
#include "collision.h"
#include "render.h"

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets;

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player, const std::vector<SDL_Texture*>& texture_vector) {

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

            int row_coord = column * half_tile + row * (-half_tile) + offset.x;
            int col_coord = column * (0.25 * tile_size) + row * (0.25 * tile_size) + offset.y;
            int grid_value = map[row][column];

            SDL_Rect destTile = { row_coord, col_coord, tile_size, tile_size };
            
            std::pair<int, int> grid_pos = {row, column};

            // cube ground
            // fixme tee mingi list vmdgi, ma ei viitsi hetkel: static const std::unordered_set<int> ground_underneath_values = {1, 2, 4, 8, 9}
            if (grid_value == 1 || grid_value == 2 || grid_value == 4 || grid_value == 8 || grid_value == 9) {
                load_cube_ground_texture(renderer, destTile);
            }

            if (grid_value == 5) {
                load_cube_snowy_ground_texture(renderer, destTile);
            }

            // green trees
            if (grid_value == 2) {
                destTile.y -= half_tile;
                render_queue.push_back(Renderable{ tree_tex, destTile, destTile.y });
            }

            // walls
            if (grid_value == 9) {
                destTile.y -= half_tile;
                render_queue.push_back(Renderable{ cube_wall_tex, destTile, destTile.y });
            }
            
            // walls w vines
            if (grid_value == 8) {
                
                // Only generate random offset once per block
                if (random_offsets.find(grid_pos) == random_offsets.end()) {
                    random_offsets[grid_pos] = rand() % 20;  // see 20 on kui palju px on maxist v2hem
                }
                int xr = random_offsets[grid_pos];

                destTile.y -= half_tile;
                destTile.y += (xr / 2);  // y-d peab eraldi lahutama, muidu block on 6hus xD
                destTile.x += (xr / 2);
                destTile.w -= xr; 
                destTile.h -= xr;

                render_queue.push_back(Renderable{ cube_wall_tex, destTile, destTile.y });
                
                destTile.y += 1;  // +1 sest muidu hakkab walli destTile'iga v6itlema ja flickerib.
                auto cube_vine_tex = choose_cube_vine_texture("", grid_pos);
                render_queue.push_back(Renderable{ cube_vine_tex, destTile, destTile.y });
            }

            // Player tile highlight (render last)
            if (row == player_tile_y && column == player_tile_x) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

                if (grid_value == 2) {
                    destTile.y += half_tile; destTile.h = half_tile;
                    SDL_RenderDrawRect(renderer, &destTile);
                } else {
                    destTile.h = half_tile;
                    SDL_RenderDrawRect(renderer, &destTile);
                }
            }
        }
    }

    // NOTE: KUI VARA PLAYER V2LJA TULEB  - half_tile
        // tuleks teha eraldi igale objektile, kus on see l2vend, et player on TOP
    int y_v = static_cast<int>(player.x * (0.25) + player.y * (0.25) + offset.y - half_tile);  
    render_queue.push_back(Renderable{ nullptr, {0,0,0,0}, y_v, [=]() { draw_player(renderer, offset); } });

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

    for (int y = 0; y < map_size; y++)
    {
        if (y < top || y > bottom)
            continue;

        for (int x = 0; x < map_size; x++)
        {
            if (x < left || x > right)
                continue;

            int row_coord = x * half_tile + y * (-half_tile) + offset.x;
            int col_coord = x * (0.25 * tile_size) + y * (0.25 * tile_size) + offset.y;
            SDL_Rect destTile = { row_coord + (tile_size / 4), col_coord, half_tile, half_tile };

            load_specific_number(renderer, map[y][x], destTile);
        }
    }
}

void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player, const std::vector<SDL_Texture*>& texture_vector)
{
    /* Vali ise, mis mappi tahad geneda. */
    render_map(renderer, offset, player, texture_vector);
    // render_map_numbers(renderer, offset, player);
}