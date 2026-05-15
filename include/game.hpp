#pragma once

#include <random>
#include <SDL3/SDL.h>
#include "game_state.hpp"
#include "common.hpp"

/* ── Game loop ────────────────────────────────────────────── */

namespace Game {
    bool init(GameState* gS);
    void load_level(GameState* gS);
    void handle_events(GameState* gS);
    void update(GameState* gS);
    void render(GameState* gS);
    bool is_running(GameState* gS);
    void shutdown(GameState* gS);
    void debug(GameState* gS, SDL_Renderer* renderer);
}
// move into state
const double PI = 3.1415926535897932384626433832;

inline float generateRandomFloat(float min, float max) {
    std::random_device rd;                               // Seed source
    std::mt19937 gen(rd());                              // Mersenne Twister engine
    std::uniform_real_distribution<float> dis(min, max); // Range [min, max)
    return dis(gen);
}


inline uint32_t make_grid_key(int row, int col) {
    return (static_cast<uint32_t>(row) << 16) | static_cast<uint32_t>(col);
}

/* functions */
void react_to_keyboard_down(GameState* gS, SDL_Keycode key);
void react_to_keyboard_up(GameState* gS, SDL_Keycode key);
void rescale_world(float oldTileSize, float newTileSize);
