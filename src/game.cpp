#include <SDL3/SDL.h>
#include <iostream>

#include "offset.hpp"
#include "game.hpp"
#include "map.hpp"
#include "maze.hpp"
#include "raycast.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "isometric_calc.hpp"


/* game state, screen */
bool isRunning = true;
float mouse_x = 0;
float mouse_y = 0;
int screen_width = 0;
int screen_height = 0;

/* framerate */
Uint32 frameCount = 0;
float fps = 0.0f;
Uint32 fpsTimer;
Uint32 frameTime;

/* tickrate */
Uint32 tickLag = 0;
const int tickrate = 60;
const int tickDelay = 1000 / tickrate;
int tickCount = 0;
float tps = 0.0f;
Uint64 previousTick = SDL_GetTicks();

/* render.hpp args */
int render_radius = 20; // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid

/* map.hpp args */
float tile_size = 100.0f;

/* pathfinding */
int pathEndX = -1;
int pathEndY = -1;
int pathStartX = -1;
int pathStartY = -1;

/* keys */
bool r_pressed = false;
bool v_pressed = false;

bool isEmpty(const SDL_FRect& r) {
    return r.w <= 0 || r.h <= 0;
}


void react_to_keyboard_down(SDL_Keycode key, struct PlayerData& player, struct Offset& offset, int map[map_size][map_size]) {
    switch (key)
    {
    case SDLK_F: {
        /* print information */
        std::cout << std::endl;

        std::cout << "----- SYSTEM -----" << "\n";
        std::cout << "offset: " << offset.x << " " << offset.y << "\n";

        std::cout << "----- PLAYER -----" << "\n";
        std::cout << "x, y: " << player.x << ", " << player.y << "\n";
        std::cout << "grid: " << static_cast<int>(player.x / tile_size) << ' ' << static_cast<int>(player.y / tile_size) \
            << " = value: " << map[static_cast<int>(player.y / tile_size)][static_cast<int>(player.x / tile_size)] << '\n';

        std::cout << "----- ENEMY -----" << "\n";
        for (const auto& e : enemyArray) {
            SDL_FPoint pos = e.get_position();
            std::cout << "x, y: " << pos.x << " " << pos.y << "\n";
            std::cout << "grid: " << e.grid.x << " " << e.grid.y << "\n";
            std::cout << "mVec: " << e.movementVector.x << " " << e.movementVector.y << "\n";
        }
        break;
    }
    case SDLK_C: {
        player.collision = !player.collision;
        std::cout << "Player collision is: " << player.collision << '\n';
        break;
    }
    case SDLK_KP_PLUS: case SDLK_PLUS: {
        render_radius += 5;
        Raycast::maxRayLength = render_radius * (tile_size * 0.75);
        Raycast::updateMaxGridSize = true;
        std::cout << "render_radius = " << render_radius << ". Raycast maxRayLength = " << Raycast::maxRayLength << "\n";
        break;
    }
    case SDLK_KP_MINUS: case SDLK_MINUS: {
        // (glade_radius > 10) ? 10 : glade_radius;  // if glade_radius > 10; hard cap to 10.
        render_radius > 5 ? render_radius -= 5 : render_radius;
        Raycast::maxRayLength = render_radius * (tile_size * 0.75);
        Raycast::updateMaxGridSize = true;
        std::cout << "render_radius = " << render_radius << ". Raycast maxRayLength = " << Raycast::maxRayLength << "\n";
        break;
    }
    case SDLK_PERIOD: {
        tile_size += 5;
        std::cout << "tile_size = " << tile_size << "\n";
        player.size = tile_size / 2;
        Raycast::updateMaxGridSize = true;
        break;
    }
    case SDLK_COMMA: {
        tile_size > 5 ? tile_size -= 5 : tile_size;
        std::cout << "tile_size = " << tile_size << "\n";
        player.size = tile_size / 2;
        Raycast::updateMaxGridSize = true;
        break;
    }
    case SDLK_PAGEDOWN: {
        pathStartX = static_cast<int>(player.x / tile_size);
        pathStartY = static_cast<int>(player.y / tile_size);
        std::cout << "Start point set: " << pathStartX << " " << pathStartY << "\n";
        break;
    }
    case SDLK_PAGEUP: {
        pathEndX = static_cast<int>(player.x / tile_size);
        pathEndY = static_cast<int>(player.y / tile_size);
        std::cout << "End point set: " << pathEndX << " " << pathEndY << "\n";
        break;
    }
    case SDLK_LSHIFT: {
        player.movement_speed = PlayerNS::DEFAULT_MOVEMENT_SPEED / 4;
        player.shifting = true;
        break;
    }
    case SDLK_R: {
        r_pressed = !r_pressed;
        std::cout << "Raycast is: " << r_pressed << '\n';
        break;
    }
    case SDLK_V: {
        v_pressed = !v_pressed;
        std::cout << "Vision is: " << v_pressed << '\n';
        break;
    }
    default:
        break;
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


void react_to_keyboard_up(SDL_Keycode key, struct PlayerData& player) {
    switch (key)
    {
    case SDLK_LSHIFT: {
        player.movement_speed = PlayerNS::DEFAULT_MOVEMENT_SPEED;
        player.shifting = false;
        break;
    }

    default:
        break;
    }
}


/// @brief This takes more resources due to if statements.
///
/// It is preferred to use other functions like `react_to_keyboard_down` and `react_to_keyboard_up`
/// as they use switch cases and react to events.
/// @param state is expected to be gotten from `SDL_GetKeyboardState(NULL)`
/// @param player struct Player player
void react_to_keyboard_state(const bool* state) {
    PlayerNS::create_movement_vector(state);
}

