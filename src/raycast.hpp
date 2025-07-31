#pragma once
#include "map.hpp"
#include "player.hpp"

#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>

#include <set>
#include <utility>

namespace Raycast {
    extern SDL_FPoint source_pos;
    extern bool show_rays;
    const int angle_step = 1;
    const int amount_of_rays = 360;
    extern std::set<std::pair<int,int>> ray_endpoints_grids;

    void update_source_pos();
    float to_radians(float degrees);
    SDL_FPoint angle_to_direction(float angle_deg);
    float calculate_line_length(int map[map_size][map_size], SDL_FPoint direction);
    void calculate(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]);
    void clear_arrays();
    void update(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]);
}