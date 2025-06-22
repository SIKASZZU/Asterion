#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "game.h"

#include "abilities.h"
#include <cmath>

Player player = {
    50, 32, 0, 0
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