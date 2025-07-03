#include <SDL2/SDL.h>
#include <iostream>
#include "game.h"

#include "abilities.h"
#include "isometric_calc.h"

/* game state, screen */
bool isRunning = true;
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

void update_offset(struct Offset& offset, struct Player& player, SDL_Window* window) {
    // convert to isometric
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    
    float row_coord = player.x * (0.5) + player.y * (-0.5);
    float col_coord = player.x * (0.25) + player.y * (0.25);

    offset.x = width / 2 - (tile_size / 2) - row_coord;
    offset.y = height / 2 - col_coord;
}


void call_set_functionality(SDL_Keycode key_pressed, struct Player& player, struct Offset& offset) {
    // std::cout << "Key pressed: " << SDL_GetKeyName(key_pressed) << " (" << key_pressed << ")\n";

    // Example: Specific key action
    if (key_pressed == SDLK_f) {
        /* print information */
        std::cout << std::endl;

        std::cout << "x, y: " << player.x << ", " << player.y << " grid: " << static_cast<int>(player.x / tile_size) << ' ' << static_cast<int>(player.y / tile_size) \
            << " = value: " << map[static_cast<int>(player.y / tile_size)][static_cast<int>(player.x / tile_size)] << '\n';
      
        std::cout << "RECT x, y: " << player.rect.x << ", " << player.rect.y << " grid: " << static_cast<int>(player.rect.x / tile_size) << ' ' << static_cast<int>(player.rect.y / tile_size) \
            << " = value: " << map[static_cast<int>(player.y / tile_size)][static_cast<int>(player.x / tile_size)] << '\n';
            
        std::cout << "offset isorectiga: " << player.rect.x + offset.x << " " << player.rect.y + offset.y << "\n";

        std::cout << "offset: " << offset.x << " " << offset.y << "\n";



        std::cout << std::endl;
    }
    
    if (key_pressed == SDLK_q) {
        std::cout << "use_melee called in call_set_functionality @ game.cpp" << '\n';
        use_melee(player);
    }
    
    if (key_pressed == SDLK_c) {
        player.collision = !player.collision;
        std::cout << "Player collision is: " << player.collision << '\n';
    }

    if (key_pressed == SDLK_e) {
        std::cout << "use_arrow called call_set_functionality @ game.cpp" << '\n';
        use_arrow(player);
    }
}