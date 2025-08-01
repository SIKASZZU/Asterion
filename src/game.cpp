#include <SDL3/SDL.h>
#include <iostream>
#include "game.hpp"
#include "map.hpp"
#include "maze.hpp"
#include "raycast.hpp"

#include "isometric_calc.hpp"

/* game state, screen */
bool isRunning = true;
float mouse_x = 0;
float mouse_y = 0;
int screen_width = 0;
int screen_height = 0;

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
bool shift_pressed = false;
bool r_pressed = false;
bool v_pressed = false;

void update_offset(struct Offset& offset, struct Player& player) {

    SDL_FPoint coords = to_isometric_coordinate(offset, player.x, player.y);

    coords.x -= offset.x;
    coords.y -= offset.y;

    offset.x = screen_width / 2 - coords.x;
    offset.y = screen_height / 2 - coords.y;
}

void react_to_keyboard_down(SDL_Keycode key, struct Player& player, struct Offset& offset, int map[map_size][map_size]) {
    switch (key)
    {
    case SDLK_F: {
        /* print information */
        std::cout << std::endl;
        std::cout << "x, y: " << player.x << ", " << player.y << " grid: " << static_cast<int>(player.x / tile_size) << ' ' << static_cast<int>(player.y / tile_size) \
            << " = value: " << map[static_cast<int>(player.y / tile_size)][static_cast<int>(player.x / tile_size)] << '\n';

        std::cout << "offset: " << offset.x << " " << offset.y << "\n";

        std::cout << "RECT x, y: " << player.rect.x << ", " << player.rect.y << "\n";
        std::cout << "RECT + offset: " << player.rect.x + offset.x << " " << player.rect.y + offset.y << "\n";
        std::cout << std::endl;
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
        player.movement_speed = DEFAULT_PLAYER_MOVEMENT_SPEED / 4;
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

void react_to_keyboard_up(SDL_Keycode key, struct Player& player) {
    switch (key)
    {
    case SDLK_LSHIFT: {
        player.movement_speed = DEFAULT_PLAYER_MOVEMENT_SPEED;
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
/// @param player 
void react_to_keyboard_state(const bool* state, struct Player& player) {
    SDL_FPoint dir = player.movement_vector;
    // std::cout << "movement_vector: " << dir.x << ' ' << dir.y << " & of dir and pMovementVec " << &dir << " " << &player.movement_vector << '\n';
    float slide_after_running = 0.25f;  // lower number = more sliding
    float sliding_threshold = 0.1f;

    if (state[SDL_SCANCODE_W]) { dir.y = -1; }
    if (state[SDL_SCANCODE_S]) { dir.y = 1; }
    if (state[SDL_SCANCODE_A]) { dir.x = -1; }
    if (state[SDL_SCANCODE_D]) { dir.x = 1; }

    if (!state[SDL_SCANCODE_W] && !state[SDL_SCANCODE_S]) {
        std::abs(dir.y) > sliding_threshold ? dir.y -= slide_after_running * (dir.y / std::abs(dir.y)) : dir.y = 0.0f;
    }
    if (!state[SDL_SCANCODE_A] && !state[SDL_SCANCODE_D]) {
        std::abs(dir.x) > sliding_threshold ? dir.x -= slide_after_running * (dir.x / std::abs(dir.x)) : dir.x = 0.0f;
    }
    player.movement_vector = { dir.x, dir.y };
}
