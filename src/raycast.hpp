#include "map.hpp"
#include "player.hpp"

#include <iostream>
#include <sdl2/SDL.h>
#include <cmath>

namespace Raycast { 
    extern int ray_length;
    const int step = 1;

    float to_radians(float degrees);
    SDL_FPoint angle_to_direction(float angle_deg);
    float calculate_line_length(SDL_FPoint direction);
    void draw(SDL_Renderer* renderer, struct Offset& offset);
}