#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "game.h"

#include "abilities.h"
#include <cmath>

Player player = {
    9, 32, 0, 0
};

bool isRunning = true;
int win_width;
int win_height;
int mouse_x = 0;
int mouse_y = 0;

/* framerate */
int frame_count = 0;
float fps = 0.0f;
Uint32 fps_timer;
Uint32 frame_start;
Uint32 frame_time;

/* tickrate */
Uint32 previous_tick = SDL_GetTicks();
Uint32 tick_lag = 0;
const int tickrate = 60;
const int tick_delay = 1000 / tickrate;
int tick_count = 0;
float ticks_per_second = 0.0f;
Uint32 tick_timer = SDL_GetTicks();
int render_radius = 100;  // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid

void render_map(SDL_Renderer* renderer, const int tile_size, struct Offset& offset,
                SDL_Texture* ground_tex, SDL_Texture* tree_tex) {
    int player_tile_y = player.y / tile_size;
    int bottom = player_tile_y + render_radius;
    int top    = player_tile_y - render_radius;

    int player_tile_x = player.x / tile_size;
    int left   = player_tile_x - render_radius;
    int right  = player_tile_x + render_radius;

    // Pass 1: Render all ground tiles
    for (int row = 0; row < map_size; row++) {
        if (row < top || row > bottom) continue;
        
        for (int column = 0; column < map_size; column++) {
            if (column < left || column > right) continue;
            
            int row_coord = row * tile_size - offset.y;
            int col_coord = column * tile_size - offset.x;
            SDL_Rect ground_tile = {col_coord, row_coord, tile_size, tile_size};

            if (map[row][column] != 0) {
                SDL_RenderCopy(renderer, ground_tex, nullptr, &ground_tile);
            }
        }
    }

    // Pass 2: Render all tree tiles w ground underneith
    for (int y = 0; y < map_size; y++) {
        if (y < top || y > bottom) continue;

        for (int x = 0; x < map_size; x++) {
            if (x < left || x > right) continue;

            if (map[y][x] == 2) {
                int row_coord = y * tile_size - offset.y;
                int col_coord = x * tile_size - offset.x;

                SDL_Rect ground_tile = {col_coord, row_coord, tile_size, tile_size};
                SDL_RenderCopy(renderer, ground_tex, nullptr, &ground_tile);
                SDL_Rect tree_tile = {
                    col_coord - (tile_size / 2),
                    row_coord - (tile_size * 2),
                    tile_size * 2,
                    tile_size * 3
                };
                SDL_RenderCopy(renderer, tree_tex, nullptr, &tree_tile);
            }
        }
    }
}


int random_number_gen(int size) {
    /* size arg = (from 1 until size) */
    if (size < 1) { size = 1; };

    srand(time(NULL));
    int number = rand() % size + 1;

    std::cout << number << '\n';

    return number;
}


void update_offset(struct Offset& offset, int win_width, int win_height) {    

    offset.x = player.x - win_width / 2;
    offset.y = player.y - win_height / 2;

    // std::cout << "OFFSET: " << offset.x << ' ' << offset.y << '\n';
}


void update_player(struct Offset& offset, const Uint8* state) {
    /* see updateimine toimub ainult playeri liikumise pealt */
    int dx = 0;
    int dy = 0;

    if (state[SDL_SCANCODE_W]) dy -= 1;
    if (state[SDL_SCANCODE_S]) dy += 1;
    if (state[SDL_SCANCODE_A]) dx -= 1;
    if (state[SDL_SCANCODE_D]) dx += 1;

    // Normalize ? input == 2 : default movement logic
    float length = std::sqrt(dx * dx + dy * dy);
    if (length != 0) {
        dx = dx / length * player.movement_speed;
        dy = dy / length * player.movement_speed;

        player.x += dx;
        player.y += dy;
    }


    // std::cout << "PLAYER: " << player.x << ' ' << player.y << '\n';

}

void call_set_functionality(SDL_Keycode key_pressed) {
    // std::cout << "Key pressed: " << SDL_GetKeyName(key_pressed) << " (" << key_pressed << ")\n";

    // Example: Specific key action
    if (key_pressed == SDLK_f) {
        int player_tile_x = player.x / tile_size;
        int player_tile_y = player.y / tile_size;
        int tile_value = map[player_tile_y][player_tile_x];
        std::cout << "Tile at (" << player_tile_y << ", " << player_tile_x << ") = " << tile_value << '\n';
    }
    
    if (key_pressed == SDLK_q) {
        std::cout << "use_melee called in call_set_functionality @ game.cpp" << '\n';
        use_melee(player.x, player.y, player.direction);
    }
    
    if (key_pressed == SDLK_e) {
        std::cout << "use_arrow called call_set_functionality @ game.cpp" << '\n';
        use_arrow(player.x, player.y, player.direction);
    }
}