#pragma once

#include <random>
#include <SDL3/SDL.h>

/* ── Game loop ────────────────────────────────────────────── */

namespace Game {
    bool init();
    void load_level();
    void handle_events();
    void update();
    void render();
    bool is_running();
    void shutdown();
    void debug(SDL_Renderer* renderer);
}

/* ── Globals (defined in game.cpp) ───────────────────────── */

// game state
extern bool  isRunning;
extern float mouse_x;
extern float mouse_y;
extern int   screenWidth;
extern int   screenHeight;

// framerate
extern Uint32 frameCount;
extern float  fps;
extern Uint32 fpsTimer;
extern Uint32 frameTime;

// tickrate
extern Uint32    tickLag;
extern const int tickrate;
extern int       tickCount;
extern float     tps;
extern Uint64    previousTick;

// world
extern int      renderRadius;
extern float    tileSize;
extern bool     testMapEnvironment;

// keys
extern bool v_pressed;

/* constants */
const double PI = 3.1415926535897932384626433832;

inline float generateRandomFloat(float min, float max) {
    std::random_device rd;                               // Seed source
    std::mt19937 gen(rd());                              // Mersenne Twister engine
    std::uniform_real_distribution<float> dis(min, max); // Range [min, max)
    return dis(gen);
}

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second << 1);
    }
};

inline uint32_t make_grid_key(int row, int col) {
    return (static_cast<uint32_t>(row) << 16) | static_cast<uint32_t>(col);
}

/* functions */
void react_to_keyboard_down(SDL_Keycode key);
void react_to_keyboard_up(SDL_Keycode key);
void rescale_world(float oldTileSize, float newTileSize);
