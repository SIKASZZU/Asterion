#pragma once
#include "map.hpp"
#include "player.hpp"
#include "isometric_calc.hpp"

#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>

namespace Raycast {
    extern SDL_FPoint source_pos;
    extern bool draw_rays;
    extern std::vector<SDL_FPoint> ray_endpoints;
    const int angle_step = 1;
    const int amount_of_rays = 360;
    
    SDL_FPoint get_iso_source_pos(struct Offset& offset);
    std::vector<SDL_FPoint>& get_ray_endpoints();
    void update_source_pos();
    float to_radians(float degrees);
    SDL_FPoint angle_to_direction(float angle_deg);
    float calculate_line_length(int map[map_size][map_size], SDL_FPoint direction);
    void calculate(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]);
    void update(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]);
}