#include "game.h"
#include <iostream>
#include "textures.h"
#include "isometric_calc.h"

void render_map(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player, 
                SDL_Texture* tree_tex, SDL_Texture* wall_tex, SDL_Texture* snowy_ground_tex) {

    SDL_FPoint player_iso = to_grid_coordinate(offset, player.x, player.y);
    
    int player_tile_x = static_cast<int>(player.x / tile_size);
    int player_tile_y = static_cast<int>(player.y / tile_size);

    int left = player_tile_x - render_radius;
    int right = player_tile_x + render_radius;
    int top = player_tile_y - render_radius;
    int bottom = player_tile_y + render_radius;

    // Pass 1: Render all ground tiles
    for (int row = 0; row < map_size; row++) {
        // if (row < top || row > bottom) continue;
        
        for (int column = 0; column < map_size; column++) {
            // if (column < left || column > right) continue;
            
            int row_coord = column * (0.5 * tile_size ) + row * (-0.5 * tile_size) + offset.x;
            int col_coord = column * (0.25 * tile_size) + row * (0.25 * tile_size) + offset.y;

            SDL_Rect destTile = {row_coord, col_coord, tile_size, tile_size};

            if (row == player_tile_y && column == player_tile_x) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // blue or green
                SDL_RenderDrawRect(renderer, &destTile);
            }

            // alumine background e ground
            if (map[row][column] != 0) {
                // load_ground_texture(renderer, destTile, row, column);
                load_snowy_ground_texture(renderer, snowy_ground_tex, destTile);
            }

            // snowy ground
            if (map[row][column] == 4){
                load_snowy_ground_texture(renderer, snowy_ground_tex, destTile);
            }
        }
    }

    // Pass 2: Render all tree tiles w ground underneith
    for (int row = 0; row < map_size; row++) {
        // if (row < top || row > bottom) continue;

        for (int column = 0; column < map_size; column++) {
            // if (column < left || column > right) continue;
               // walls
            if (map[row][column] == 9){
                int row_coord = column * (0.5 * tile_size ) + row * (-0.5 * tile_size) + offset.x;
                int col_coord = column * (0.25 * tile_size) + row * (0.25 * tile_size) + offset.y;
                SDL_Rect destTile = {row_coord, col_coord, tile_size, tile_size};
                load_wall_texture(renderer, wall_tex, map, row, column, destTile);
            }

            if (map[row][column] == 2){
                // int row_coord = column * (0.5 * tile_size ) + row *(-0.5 * tile_size) + offset.x;
                // int col_coord = column * (0.25 * tile_size) + row *(0.25 * tile_size) + offset.y;

                // SDL_Rect tree_tile = {
                //     row_coord - (tile_size * 2),
                //     col_coord - (tile_size / 2),
                //     tile_size * 2,
                //     tile_size * 3
                // };
                // SDL_RenderCopy(renderer, tree_tex, nullptr, &tree_tile);
            }
        }
    }
}


void render_map_numbers(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player) {
    int player_tile_y = player.y / tile_size;
    int bottom = player_tile_y + render_radius;
    int top    = player_tile_y - render_radius;

    int player_tile_x = player.x / tile_size;
    int left   = player_tile_x - render_radius;
    int right  = player_tile_x + render_radius;

    for (int y = 0; y < map_size; y++) {
        if (y < top || y > bottom) continue;

        for (int x = 0; x < map_size; x++) {
            if (x < left || x > right) continue;
            
            int row_coord = y * tile_size + offset.x;
            int col_coord = x * tile_size + offset.y;
            SDL_Rect destTile = {col_coord, row_coord, tile_size, tile_size};
            load_specific_number(renderer, map[y][x], destTile);
        }
    }
}


void load_render(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player) {
    /* Vali ise, mis mappi tahad geneda. */
    render_map(renderer, tile_size, offset, player, tree_tex, wall_tex, snowy_ground_tex);
    // render_map_numbers(renderer, tile_size, offset, player);
}