#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "game.h"

#include "abilities.h"
#include <cmath>
#include <random>
#include "isometric_calc.h"

Player player = {
    5, 32, 0, 0
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
int render_radius = 20;  // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid


int random_number_gen(int min, int max) {
    /* args int min, int max */
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(min, max - 1);
    return dist(rng);
}

void update_offset(struct Offset& offset, int win_width, int win_height) {
    offset.x = win_width / 2 - tile_size / 2;
    offset.y = win_height / 2 - tile_size / 2;
}


void update_player(struct Offset& offset, const Uint8* state) {
    float dx = 0;
    float dy = 0;

    // Normal map inputs
    // if (state[SDL_SCANCODE_W]) dy -= 1;
    // if (state[SDL_SCANCODE_S]) dy += 1;
    // if (state[SDL_SCANCODE_A]) dx -= 1;
    // if (state[SDL_SCANCODE_D]) dx += 1;

    // // isometric directions, broken veits
    if (state[SDL_SCANCODE_W]) { dx -= 1; dy += 0.5; }
    if (state[SDL_SCANCODE_S]) { dx += 1; dy -= 0.5; }
    if (state[SDL_SCANCODE_A]) { dx -= 0.5; dy -= 1; }
    if (state[SDL_SCANCODE_D]) { dx += 0.5; dy += 1; }

    // Normalize movement vector
    float length = std::sqrt(dx * dx + dy * dy);
    if (length != 0) {
        dx = dx / length * player.movement_speed;
        dy = dy / length * player.movement_speed;

        player.x += dy;
        player.y += dx;
    }
}

void call_set_functionality(SDL_Keycode key_pressed, struct Offset& offset) {
    // std::cout << "Key pressed: " << SDL_GetKeyName(key_pressed) << " (" << key_pressed << ")\n";

    // Example: Specific key action
    if (key_pressed == SDLK_f) {
        std::cout << std::endl;

        int player_tile_x = player.x / tile_size;
        int player_tile_y = player.y / tile_size;
        int tile_value = map[player_tile_y][player_tile_x];
        std::cout << "X, Y " << player.x << ", " << player.y << " = value: " << tile_value << '\n';
        std::cout << "Tile at " << player_tile_y << ", " << player_tile_x << " = " << tile_value << '\n';
 
        SDL_Point iso_player = to_grid_coordinate(player.x, player.y);

        std::cout << "iso " << iso_player.x << ", " << iso_player.y << " = " << iso_player.x / tile_size << ' ' << iso_player.y / tile_size << '\n';
        // std::cout << "Tile at (" << player_tile_y << ", " << player_tile_x << ") = " << tile_value << '\n';
        
        std::cout << std::endl;
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