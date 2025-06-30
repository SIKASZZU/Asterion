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
    10, 32, 0, 0
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
    // convert to isometric
    float row_coord = player.x * (0.5) + player.y * (-0.5);
    float col_coord = player.x * (0.25) + player.y * (0.25);

    offset.x = win_width / 2 - (tile_size / 2) - row_coord;
    offset.y = win_height / 2 - col_coord;
}


void update_player(struct Offset& offset, const Uint8* state, SDL_Renderer* renderer) {
    SDL_FPoint dir = {0,0};

    if (state[SDL_SCANCODE_W]) { dir.y -= 1; }
    if (state[SDL_SCANCODE_S]) { dir.y += 1; }
    if (state[SDL_SCANCODE_A]) { dir.x -= 1; }
    if (state[SDL_SCANCODE_D]) { dir.x += 1; }

    dir.x = dir.x * player.movement_speed;
    dir.y = dir.y * player.movement_speed;
    
    player.x += dir.x;
    player.y += dir.y;
    
    // convert to isometric
    float row_coord = player.x * (0.5) + player.y * (-0.5) + offset.x;
    float col_coord = player.x * (0.25) + player.y * (0.25) + offset.y;

    // ma liidan (tile_size / 4) et player tile'i keskele saada. Eeldusega, et tile size ja player size on harmoonias ehk 100 ja 25 n2itkes mitte 100 ja 13  
    SDL_FRect player_rect = {row_coord  + (tile_size / 4), col_coord, static_cast<float>(player.size), static_cast<float>(player.size)};
    SDL_RenderFillRectF(renderer, &player_rect);
}

void call_set_functionality(SDL_Keycode key_pressed) {
    // std::cout << "Key pressed: " << SDL_GetKeyName(key_pressed) << " (" << key_pressed << ")\n";

    // Example: Specific key action
    if (key_pressed == SDLK_f) {
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