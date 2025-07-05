#include "game.h"
#include "textures.h"
#include "collision.h"
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

int render_radius = 5; // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid

void draw_nearby(SDL_Renderer* renderer, struct Offset& offset, SDL_Point zero_grid) {

    auto calculate_tile_coords = [&](int x, int y) {
        int row_coord = x * (0.5 * tile_size) + y * (-0.5 * tile_size) + tile_size / 4 + offset.x;
        int col_coord = x * (0.25 * tile_size) + y * (0.25 * tile_size) + offset.y;

        return SDL_Point{ row_coord, col_coord };
        };

    int half_tile = tile_size / 2;
    SDL_Rect nearbyTile;

    SDL_SetRenderDrawColor(renderer, 255, 193, 203, 255); //pink -> mida peab renderima enne playerit

    SDL_Point left_coords = calculate_tile_coords(zero_grid.x - 1, zero_grid.y);
    nearbyTile = { left_coords.x, left_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point up_coords = calculate_tile_coords(zero_grid.x, zero_grid.y - 1);
    nearbyTile = { up_coords.x, up_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point leftup_coords = calculate_tile_coords(zero_grid.x - 1, zero_grid.y - 1);
    nearbyTile = { leftup_coords.x, leftup_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point leftdown_coords = calculate_tile_coords(zero_grid.x - 1, zero_grid.y + 1);
    nearbyTile = { leftdown_coords.x, leftdown_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point rightup_coords = calculate_tile_coords(zero_grid.x + 1, zero_grid.y - 1);
    nearbyTile = { rightup_coords.x, rightup_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_SetRenderDrawColor(renderer, 193, 65, 255, 255); //magneta -> mida peab renderima peale playerit

    SDL_Point down_coords = calculate_tile_coords(zero_grid.x, zero_grid.y + 1);
    nearbyTile = { down_coords.x, down_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point right_coords = calculate_tile_coords(zero_grid.x + 1, zero_grid.y);
    nearbyTile = { right_coords.x, right_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);

    SDL_Point rightdown_coords = calculate_tile_coords(zero_grid.x + 1, zero_grid.y + 1);
    nearbyTile = { rightdown_coords.x, rightdown_coords.y, half_tile, half_tile };
    SDL_RenderDrawRect(renderer, &nearbyTile);
}


void render_map(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player,
    SDL_Texture* tree_tex, SDL_Texture* wall_tex, SDL_Texture* cube_ground_tex) {

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

    for (int row = 0; row < map_size; row++) {
        if (row < top || row > bottom) continue;

        for (int column = 0; column < map_size; column++) {
            if (column < left || column > right) continue;

            int row_coord = column * (0.5 * tile_size) + row * (-0.5 * tile_size) + offset.x;
            int col_coord = column * (0.25 * tile_size) + row * (0.25 * tile_size) + offset.y;
            int grid_value = map[row][column];

            SDL_Rect destTile = { row_coord, col_coord, tile_size, tile_size };

            // cube ground
            if (grid_value == 4) {
                load_cube_ground_texture(renderer, cube_ground_tex, destTile);
            }

            // green trees
            if (grid_value == 2) {
                destTile.y -= (tile_size / 2);
                render_queue.push_back(Renderable{ tree_tex, destTile, destTile.y });
            }

            // walls
            if (grid_value == 9) {
                destTile.y -= (tile_size / 2);
                render_queue.push_back(Renderable{ wall_tex, destTile, destTile.y });
            }

            // Player tile highlight (render last)
            if (row == player_tile_y && column == player_tile_x) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                destTile.h = (tile_size / 2);
                SDL_RenderDrawRect(renderer, &destTile);
            }
        }
    }

    // NOTE: KUI VARA PLAYER V2LJA TULEB  - (tile_size * 0.5)
        // tuleks teha eraldi igale objektile, kus on see l2vend, et player on TOP
    int y_v = static_cast<int>(player.x * (0.25) + player.y * (0.25) + offset.y - (tile_size * 0.5));  
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


void render_map_numbers(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player)
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
            SDL_Rect destTile = { row_coord + (tile_size / 4), col_coord, tile_size / 2, tile_size / 2 };

            load_specific_number(renderer, map[y][x], destTile);
        }
    }
}

void load_render(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player)
{
    /* Vali ise, mis mappi tahad geneda. */
    render_map(renderer, tile_size, offset, player, tree_tex, wall_tex, cube_ground_tex);
    // render_map_numbers(renderer, tile_size, offset, player);
}