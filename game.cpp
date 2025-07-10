#include <SDL2/SDL.h>
#include <iostream>
#include "game.h"
#include "map.h"
#include "maze.h"

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

/* render.h args */
int render_radius = 20; // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid

/* map.h args */
int tile_size = 25; 

/* pathfinding */
int pathEndX = -1;
int pathEndY = -1;
int pathStartX = -1;
int pathStartY = -1;

/* keys */
bool shift_pressed = false;

void update_offset(struct Offset& offset, struct Player& player, SDL_Window* window) {
    // convert to isometric
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    
    float row_coord = player.x * (0.5) + player.y * (-0.5);
    float col_coord = player.x * (0.25) + player.y * (0.25);

    offset.x = width / 2 - (tile_size / 2) - row_coord;
    offset.y = height / 2 - col_coord;
}


void call_set_functionality(SDL_Keycode key_pressed, struct Player& player, struct Offset& offset, int map[map_size][map_size]) {
    // std::cout << key_pressed << "\n";

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
    
    if (key_pressed == SDLK_e) {
        std::cout << "use_arrow called call_set_functionality @ game.cpp" << '\n';
        use_arrow(player);
    }

    /* turn player collison off & on*/
    if (key_pressed == SDLK_c) {
        player.collision = !player.collision;
        std::cout << "Player collision is: " << player.collision << '\n';
    }

    /* plus to increase render_radius; minus to reduce render_radius*/
    if (key_pressed == SDLK_KP_PLUS || key_pressed == SDLK_PLUS) {
        render_radius += 5;
        std::cout << "render_radius = " << render_radius << "\n";
    } 
    
    if (key_pressed == SDLK_KP_MINUS || key_pressed == SDLK_MINUS) {
        // (glade_radius > 10) ? 10 : glade_radius;  // if glade_radius > 10; hard cap to 10.
        render_radius > 5 ? render_radius -= 5 : render_radius;
        std::cout << "render_radius = " << render_radius << "\n";
    }

    /* . to reduce tile_size; , to increase tile_size */
    if (key_pressed == SDLK_PERIOD) {
        tile_size += 5;
        std::cout << "tile_size = " << tile_size << "\n";
    }

    if (key_pressed == SDLK_COMMA) {
        tile_size > 5 ? tile_size -= 5 : tile_size;
        std::cout << "tile_size = " << tile_size << "\n";
    }
    
    /* pathfinding keys -> p to set end; o to set start*/
    if (key_pressed == SDLK_p) {
        pathEndX = static_cast<int>(player.x / tile_size);
        pathEndY = static_cast<int>(player.y / tile_size);
        std::cout << "End point set: " << pathEndX << " " << pathEndY << "\n";
    }

    if (key_pressed == SDLK_o) {
        pathStartX = static_cast<int>(player.x / tile_size);
        pathStartY = static_cast<int>(player.y / tile_size);
        std::cout << "Start point set: " << pathStartX << " " << pathStartY << "\n";
    }
    
    if (pathEndX != -1 && pathStartX != -1) {
        Maze::find_path(map, pathStartY, pathStartX, pathEndY, pathEndX); // dont ask miks need tagurpidi on
        
        // reset
        pathEndX = -1;
        pathEndY = -1;
        pathStartX = -1;
        pathStartY = -1;
    }

}