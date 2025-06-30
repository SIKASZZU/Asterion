#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "game.h"

#include "abilities.h"
#include <cmath>
#include <random>
#include "isometric_calc.h"

/* game state, screen */
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

void update_offset(struct Offset& offset, struct Player& player, int win_width, int win_height) {
    // convert to isometric
    float row_coord = player.x * (0.5) + player.y * (-0.5);
    float col_coord = player.x * (0.25) + player.y * (0.25);

    offset.x = win_width / 2 - (tile_size / 2) - row_coord;
    offset.y = win_height / 2 - col_coord;
}


void call_set_functionality(SDL_Keycode key_pressed, struct Player& player) {
    // std::cout << "Key pressed: " << SDL_GetKeyName(key_pressed) << " (" << key_pressed << ")\n";

    // Example: Specific key action
    if (key_pressed == SDLK_f) {
        /* print information */
        std::cout << std::endl;

        std::cout << "x, y: " << player.x << ", " << player.y << " grid: " << static_cast<int>(player.x / tile_size) << ' ' << static_cast<int>(player.y / tile_size) \
            << " = value: " << map[static_cast<int>(player.y / tile_size)][static_cast<int>(player.x / tile_size)] << '\n';
      
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