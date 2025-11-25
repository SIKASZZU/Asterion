#pragma once // compilerile relvaga kulpi, et jooksutaks seda faili vaid yhe korra
#include <SDL3/SDL.h>
#include <functional>

#include "offset.hpp"
#include "player.hpp"
#include "map.hpp"
#include "enemy.hpp"

// /* screen */
extern int screenWidth;
extern int screenHeight;
extern float mouse_x;
extern float mouse_y;

/* game */
extern bool isRunning;
extern const Uint8* state;

/* framerate */
extern Uint32 frameTime;
extern Uint32 frameCount;
extern float fps;
extern Uint32 fpsTimer;

/* tickrate */
extern const int tickrate;
extern const int tickDelay;
extern Uint32 tickLag;
extern int tickCount;
extern float tps;
extern Uint64 previousTick;
const Uint64 TICK_DELAY_MS = 16;  // 60 ticks per second
const float fixedDeltaTime = TICK_DELAY_MS / 1000.0f;  // 0.016

/* render.hpp args */
extern int renderRadius;

/* map.hpp args */
extern float tileSize;

/* pathfinding */
extern int pathEndX;
extern int pathEndY;
extern int pathStartX;
extern int pathStartY;

/* keys */
extern bool r_pressed;
extern bool v_pressed;

/* constants */
const double PI = 3.1415926535897932384626433832;

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second << 1);
    }
};

/* functions */
bool isEmpty(const SDL_FRect& r);
void react_to_keyboard_down(SDL_Keycode key, struct PlayerData& player, struct Offset& offset, int map[mapSize][mapSize]);
void react_to_keyboard_up(SDL_Keycode key, struct PlayerData& player);
void react_to_keyboard_state(const bool* state);
void rescale_world_after_tilesize_change(float oldTileSize, float newTileSize);
// Day/night system
extern bool dayNightEnabled;
extern float timeOfDay; // 0.0 - 1.0
extern float dayLengthSeconds; // seconds for a full day cycle
void update_daynight(Uint64 elapsedMS);
// Game settings struct (runtime-configurable and easy to serialize)
struct GameSettings {
    bool enableDayNight = true;
    float defaultDayLengthSeconds = 120.0f;
    bool invertDayNight = false;
};
extern GameSettings gameSettings;
// returns brightness 0..1 where 1 = day (bright), 0 = night (dark)
float get_day_brightness();