#include "game.h"
#include "textures.h"
#include "collision.h"
 
int render_radius = 5; // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid

void render_map(SDL_Renderer *renderer, const int tile_size, struct Offset &offset, struct Player &player,
                SDL_Texture *tree_tex, SDL_Texture *wall_tex, SDL_Texture *cube_ground_tex) {

    int player_tile_x = static_cast<int>(player.x / tile_size);
    int player_tile_y = static_cast<int>(player.y / tile_size);

    int left = player_tile_x - render_radius;
    int right = player_tile_x + render_radius;
    int top = player_tile_y - render_radius;
    int bottom = player_tile_y + render_radius;

    // Pass 1: Render all ground tiles
    for (int row = 0; row < map_size; row++) {
        if (row < top || row > bottom) continue;

        for (int column = 0; column < map_size; column++) {
            if (column < left || column > right) continue;

            int row_coord = column * (0.5 * tile_size) + row * (-0.5 * tile_size) + offset.x;
            int col_coord = column * (0.25 * tile_size) + row * (0.25 * tile_size) + offset.y;

            SDL_Rect destTile = {row_coord, col_coord, tile_size, tile_size};

            // draw player rect
            if (row == player_tile_y && column == player_tile_x) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // blue or green
                SDL_RenderDrawRect(renderer, &destTile);
            }

            // alumine background e ground
            if (map[row][column] != 9 || map[row][column] != 0) {
                // load_ground_texture(renderer, destTile, row, column);
                load_cube_ground_texture(renderer, cube_ground_tex, destTile);
            }

            // // cube ground
            // if (map[row][column] == 4) {
            //     load_cube_ground_texture(renderer, cube_ground_tex, destTile);
            // }

            // walls
            if (map[row][column] == 9) {
                destTile.y -= (tile_size / 2);
                load_cube_wall_texture(renderer, wall_tex, map, row, column, destTile);
            } 
        }
    }

    // // Pass 2: Render all tree tiles w ground underneith
    // for (int row = 0; row < map_size; row++) {
    //     if (row < top || row > bottom) continue;

    //     for (int column = 0; column < map_size; column++) {
    //         if (column < left || column > right) continue;

    //         int row_coord = column * (0.5 * tile_size ) + row * (-0.5 * tile_size) + offset.x;
    //         int col_coord = column * (0.25 * tile_size) + row * (0.25 * tile_size) + offset.y;

    //         SDL_Rect destTile = {row_coord, col_coord, tile_size, tile_size};

    //         // walls
    //         if (map[row][column] == 9){
    //             load_cube_wall_texture(renderer, wall_tex, map, row, column, destTile);
    //         }

    //         if (map[row][column] == 2){
    //             // int row_coord = column * (0.5 * tile_size ) + row *(-0.5 * tile_size) + offset.x;
    //             // int col_coord = column * (0.25 * tile_size) + row *(0.25 * tile_size) + offset.y;

    //             // SDL_Rect tree_tile = {
    //             //     row_coord - (tile_size * 2),
    //             //     col_coord - (tile_size / 2),
    //             //     tile_size * 2,
    //             //     tile_size * 3
    //             // };
    //             // SDL_RenderCopy(renderer, tree_tex, nullptr, &tree_tile);
    //         }
    //     }
    // }
}

void render_map_numbers(SDL_Renderer *renderer, const int tile_size, struct Offset &offset, struct Player &player)
{
    int player_tile_y = player.y / tile_size;
    int bottom = player_tile_y + render_radius;
    int top = player_tile_y - render_radius;

    int player_tile_x = player.x / tile_size;
    int left = player_tile_x - render_radius;
    int right = player_tile_x + render_radius;

    for (int y = 0; y < map_size; y++)
    {
        if (y < top || y > bottom)
            continue;

        for (int x = 0; x < map_size; x++)
        {
            if (x < left || x > right)
                continue;

            int row_coord = x * (0.5 * tile_size) + y * (-0.5 * tile_size) + offset.x;
            int col_coord = x * (0.25 * tile_size) + y * (0.25 * tile_size) + offset.y;
            SDL_Rect destTile = {row_coord + (tile_size / 4), col_coord, tile_size / 2, tile_size / 2};

            load_specific_number(renderer, map[y][x], destTile);
        }
    }
}

void load_render(SDL_Renderer *renderer, const int tile_size, struct Offset &offset, struct Player &player)
{
    /* Vali ise, mis mappi tahad geneda. */
    render_map(renderer, tile_size, offset, player, tree_tex, wall_tex, cube_ground_tex);
    // render_map_numbers(renderer, tile_size, offset, player);
}