#pragma once
#include "map.hpp"
#include "player.hpp"

#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>
#include <deque>
#include <set>
#include <utility>

namespace Raycast {
    extern SDL_FPoint sourcePos;
    extern bool showRays;
    const int increment = 1;
    const int angleStep = 1;
    extern signed int maxActiveSize;
    extern signed int maxDecaySize;
    extern bool updateMaxGridSize;
    extern std::set<std::pair<int, int>> endpointActiveGrids;
    extern std::set<std::pair<int, int>> activeGridsMaxSize;
    extern std::deque<std::pair<int, int>> decayGrids;

    void update_sourcePos();
    float to_radians(float degrees);
    SDL_FPoint angle_to_direction(float angle_deg);
    float calculate_line_length(int map[map_size][map_size], SDL_FPoint direction);
    void calculate_active_grids(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]);
    void calculate_decay_grids();
    void clear_arrays();
    void update_max_grid_size();
    void update(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]);
}