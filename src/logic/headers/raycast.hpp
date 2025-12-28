#pragma once
#include "map.hpp"
#include "player.hpp"
#include "offset.hpp"

#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>
#include <deque>
#include <set>
#include <utility>
#include <thread>
#include <vector>
#include <condition_variable>

namespace Raycast {
    extern bool enabled;
    extern bool showRays;
    extern SDL_FPoint sourcePos;
    const int increment = 3;
    const int angleStep = 1;
    extern signed int maxActiveSize;
    extern signed int maxDecaySize;
    extern float maxRayLength;
    extern bool updateMaxGridSize;
    extern std::set<std::pair<int, int>> endpointActiveGrids;
    extern std::set<std::pair<int, int>> activeGridsMaxSize;
    extern std::deque<std::pair<int, int>> decayGrids;

    // Threading API: start/stop worker, request calculation, render results
    std::jthread::id start_worker();
    void stop_worker();
    void request_calculation();
    void wait_until_ready();
    void render_rays(SDL_Renderer* renderer, struct Offset& offset);

    void update_sourcePos();
    float to_radians(float degrees);
    SDL_FPoint angle_to_direction(float angle_deg);
    void calculate_active_grids(const SDL_FPoint& srcPos);
    void calculate_decay_grids();
    void clear_arrays();
    void update_max_grid_size();
    void update(SDL_Renderer* renderer, struct Offset& offset);
}