#include "game.h"
#include "textures.h"
#include "collision.h"
 
int render_radius = 5; // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid

void draw_nearby(SDL_Renderer *renderer, struct Offset& offset, SDL_Point zero_grid) {
    
    auto calculate_tile_coords = [&](int x, int y) {
        int row_coord = x * (0.5 * tile_size) + y * (-0.5 * tile_size) + tile_size / 4 + offset.x;
        int col_coord = x * (0.25 * tile_size) + y * (0.25 * tile_size)  + offset.y;
        
        return SDL_Point{row_coord, col_coord};
    };
    
    int half_tile = tile_size / 2;
    SDL_Rect nearbyTile;
    
    SDL_SetRenderDrawColor(renderer, 255, 193, 203, 255); //pink -> mida peab renderima enne playerit
    
    SDL_Point left_coords = calculate_tile_coords(zero_grid.x - 1, zero_grid.y);
    nearbyTile = {left_coords.x, left_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);
    
    SDL_Point up_coords = calculate_tile_coords(zero_grid.x, zero_grid.y - 1);
    nearbyTile = {up_coords.x, up_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);
    
    SDL_Point leftup_coords = calculate_tile_coords(zero_grid.x - 1, zero_grid.y - 1);
    nearbyTile = {leftup_coords.x, leftup_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);
    
    SDL_Point leftdown_coords = calculate_tile_coords(zero_grid.x - 1, zero_grid.y + 1);
    nearbyTile = {leftdown_coords.x, leftdown_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point rightup_coords = calculate_tile_coords(zero_grid.x + 1, zero_grid.y - 1);
    nearbyTile = {rightup_coords.x, rightup_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);
    
    SDL_SetRenderDrawColor(renderer, 193, 65, 255, 255); //magneta -> mida peab renderima peale playerit

    SDL_Point down_coords = calculate_tile_coords(zero_grid.x, zero_grid.y + 1);
    nearbyTile = {down_coords.x, down_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point right_coords = calculate_tile_coords(zero_grid.x + 1, zero_grid.y);
    nearbyTile = {right_coords.x, right_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point rightdown_coords = calculate_tile_coords(zero_grid.x + 1, zero_grid.y + 1);
    nearbyTile = {rightdown_coords.x, rightdown_coords.y, half_tile, half_tile};
    SDL_RenderDrawRect(renderer, &nearbyTile);
}


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

            // note: kui tekstuuri tahta tosta maa peale siis destTile.y -= (tile_size * 0.5);
            // note: 1 kord destTile.y -= (tile_size * 0.5); on v6rdeline 1 korrusega. Lahuta 1x veel, ss block spawnib 2ne korrus.
            
            SDL_Rect destTile = {row_coord, col_coord, tile_size, tile_size};

            // alumine background e ground
            if (map[row][column] != 9 || map[row][column] != 0) {
                // load_ground_texture(renderer, destTile, row, column);
                load_cube_ground_texture(renderer, cube_ground_tex, destTile);
            }

            // // cube ground
            // if (map[row][column] == 4) {
            //     load_cube_ground_texture(renderer, cube_ground_tex, destTile);
            // }

            // green trees
            if (map[row][column] == 2){
                destTile.y -= (tile_size / 2);
                // destTile.w *= 2; 
                // destTile.h *= 3;
                SDL_RenderCopy(renderer, tree_tex, nullptr, &destTile);
            }

            // walls
            if (map[row][column] == 9) {
                destTile.y -= (tile_size * 0.5);
                load_cube_wall_texture(renderer, wall_tex, map, row, column, destTile); // first layer
                // destTile.y -= (tile_size * 0.5);
                // load_cube_wall_texture(renderer, wall_tex, map, row, column, destTile); // second layer
                
            } 


            }

        }

    // Pass 2: Render all tree tiles w ground underneith
    for (int row = 0; row < map_size; row++) {
        if (row < top || row > bottom) continue;

        for (int column = 0; column < map_size; column++) {
            if (column < left || column > right) continue;

            int row_coord = column * (0.5 * tile_size) + row * (-0.5 * tile_size) + offset.x;
            int col_coord = column * (0.25 * tile_size) + row * (0.25 * tile_size) + offset.y;

            SDL_Rect destTile = {row_coord, col_coord, tile_size, tile_size};
            // player rect && nearby tiles
            if (row == player_tile_y && column == player_tile_x) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
                SDL_RenderDrawRect(renderer, &destTile);
                
                SDL_Point player_grid = {player_tile_x, player_tile_y};
                draw_nearby(renderer, offset, player_grid);
            }
            // okei, trying to cook some shit up real bad.
            // idee et renderib peale playerit! kui parem idee, siis see on rm rf ./
            if (column == player_tile_x && row == (player_tile_y + 1) ||
                column == (player_tile_x + 1) && row == player_tile_y ||
                column == (player_tile_x + 1) && row == (player_tile_y + 1)) {
                
                // walls first layer
                if (map[row][column] == 9) {
                    destTile.y -= (tile_size * 0.5);
                    load_cube_wall_texture(renderer, wall_tex, map, row, column, destTile);
                }

                // green trees
                if (map[row][column] == 2){
                    destTile.y -= (tile_size / 2);
                    // destTile.w *= 2; 
                    // destTile.h *= 3;
                    SDL_RenderCopy(renderer, tree_tex, nullptr, &destTile);
                }
            }

        }
    }
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