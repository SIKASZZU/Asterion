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
#include "daylight.hpp"

/* game state, screen */
bool isRunning = true;
float mouse_x = 0;
float mouse_y = 0;
int screenWidth = 0;
int screenHeight = 0;

/* framerate */
Uint32 frameCount = 0;
float fps = 0.0f;
Uint32 fpsTimer;
Uint32 frameTime;

/* tickrate */
Uint32 tickLag = 0;
const int tickrate = 60;
int tickCount = 0;
float tps = 0.0f;
Uint64 previousTick = SDL_GetTicks();

/* render.hpp args */
int renderRadius = 20; // perfectse rad -> (win_width / 2) / tileSize //*NOTE win_widthil pole siin veel v22rtust vaid

/* map.hpp args */
float tileSize = 100.0f;

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

// Recalculate world-space positions after `tileSize` changes.
// oldTileSize: previous tile size; newTileSize: current `tileSize` global
void rescale_world_after_tilesize_change(float oldTileSize, float newTileSize) {
    // Player: compute grid from old pixel coords and reproject to new tileSize
    int playerGX = static_cast<int>((player.x + player.size / 2.0f) / oldTileSize);
    int playerGY = static_cast<int>((player.y + player.size / 2.0f) / oldTileSize);
    player.x = playerGX * newTileSize;
    player.y = playerGY * newTileSize;
    player.size = newTileSize / 2.0f;
    player.gridX = playerGX;
    player.gridY = playerGY;
    SDL_FPoint coords = to_isometric_coordinate(player.x, player.y);
    player.rect.x = coords.x + player.size / 2.0f;
    player.rect.y = coords.y - player.size / 2.0f;

    for (auto &e : enemyArray) {
        SDL_FPoint newPos = {static_cast<float>(e.grid.x) * newTileSize, static_cast<float>(e.grid.y) * newTileSize};
        e.set_position(newPos);
    }
}


void react_to_keyboard_down(SDL_Keycode key, struct PlayerData& player, struct Offset& offset, int map[mapSize][mapSize]) {
    switch (key)
    {
    case SDLK_F: {
        /* print information */
        std::cout << std::endl;

        std::cout << "----- SYSTEM -----" << "\n";
        std::cout << "offset: " << offset.x << " " << offset.y << "\n";

        std::cout << "----- PLAYER -----" << "\n";
        std::cout << "x, y: " << player.x << ", " << player.y << "\n";
        std::cout << "grid: " << static_cast<int>(player.x / tileSize) << ' ' << static_cast<int>(player.y / tileSize) \
            << " = value: " << map[static_cast<int>(player.y / tileSize)][static_cast<int>(player.x / tileSize)] << '\n';
        std::cout << "movementSpeed: " << player.movementSpeed << std::endl;

        std::cout << "----- ENEMY -----" << "\n";
        for (const auto& e : enemyArray) {
            SDL_FPoint pos = e.get_position();
            std::cout << "x, y: " << pos.x << " " << pos.y << "\n";
            std::cout << "activity: " << e.activity << "\n";
            std::cout << "grid: " << e.grid.x << " " << e.grid.y << "\n";
            SDL_Point mV = e.get_movementVector(); 
            std::cout << "mVec: " << mV.x << " " << mV.y << std::endl;
        }
        break;
    }
    case SDLK_C: {
        player.collision = !player.collision;
        std::cout << "Player collision is: " << player.collision << '\n';
        break;
    }
    case SDLK_KP_PLUS: case SDLK_PLUS: {
        renderRadius += 5;
        Raycast::maxRayLength = renderRadius * (tileSize * 0.75);
        Raycast::updateMaxGridSize = true;
        std::cout << "renderRadius = " << renderRadius << ". Raycast maxRayLength = " << Raycast::maxRayLength << "\n";
        break;
    }
    case SDLK_KP_MINUS: case SDLK_MINUS: {
        // (glade_radius > 10) ? 10 : glade_radius;  // if glade_radius > 10; hard cap to 10.
        renderRadius > 5 ? renderRadius -= 5 : renderRadius;
        Raycast::maxRayLength = renderRadius * (tileSize * 0.75);
        Raycast::updateMaxGridSize = true;
        std::cout << "renderRadius = " << renderRadius << ". Raycast maxRayLength = " << Raycast::maxRayLength << "\n";
        break;
    }
    case SDLK_PERIOD: {
        {
            float oldTs = tileSize;
            tileSize += 5;
            std::cout << "tileSize = " << tileSize << "\n";
            Raycast::updateMaxGridSize = true;
            // rescale world to keep logical positions
            rescale_world_after_tilesize_change(oldTs, tileSize);
        }
        break;
    }
    case SDLK_COMMA: {
        {
            float oldTs = tileSize;
            tileSize > 5 ? tileSize -= 5 : tileSize;
            std::cout << "tileSize = " << tileSize << "\n";
            Raycast::updateMaxGridSize = true;
            rescale_world_after_tilesize_change(oldTs, tileSize);
        }
        break;
    }
    case SDLK_PAGEDOWN: {
        pathStartX = static_cast<int>(player.x / tileSize);
        pathStartY = static_cast<int>(player.y / tileSize);
        std::cout << "Start point set: " << pathStartX << " " << pathStartY << "\n";
        break;
    }
    case SDLK_PAGEUP: {
        pathEndX = static_cast<int>(player.x / tileSize);
        pathEndY = static_cast<int>(player.y / tileSize);
        std::cout << "End point set: " << pathEndX << " " << pathEndY << "\n";
        break;
    }
    case SDLK_LSHIFT: {
        player.movementSpeed = PlayerNS::defaultMovementSpeed / 4;
        player.shifting = true;
        break;
    }
    case SDLK_R: {
        r_pressed = !r_pressed;
        std::cout << "Raycast is: " << r_pressed << '\n';
        break;
    }
    case SDLK_Y: {
        // enable day
        daylightSettings.daylightEnabled = !daylightSettings.daylightEnabled;
        std::cout << "Day/Night enabled: " << daylightSettings.daylightEnabled << " timeOfDay: " << DaylightNS::timeOfDay << '\n';
        break;
    }
    case SDLK_U: {
        // shorten day
        daylightSettings.dayLengthSeconds = std::max(5.0f, daylightSettings.dayLengthSeconds - 10.0f);
        std::cout << "dayLengthSeconds = " << daylightSettings.dayLengthSeconds << '\n';
        break;
    }
    case SDLK_I: {
        // lengthen day
        daylightSettings.dayLengthSeconds += 10.0f;
        std::cout << "dayLengthSeconds = " << daylightSettings.dayLengthSeconds << '\n';
        break;
    }
    case SDLK_V: {
        v_pressed = !v_pressed;
        std::cout << "Vision is: " << v_pressed << '\n';
        break;
    }
    case SDLK_Q: {
        std::cout << "Forced exit " << std::endl;
        isRunning = false;
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
        player.movementSpeed = PlayerNS::defaultMovementSpeed;
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
    PlayerNS::create_movementVector(state);
}

