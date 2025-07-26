#include "map.hpp"
#include "player.hpp"

#include <iostream>
#include <sdl2/SDL.h>
#include <cmath>

namespace Raycast { 
    extern int ray_length;
    extern SDL_FPoint source_pos;
    const int angle_step = 1;

    void update_source_pos();
    float to_radians(float degrees);
    SDL_FPoint angle_to_direction(float angle_deg);
    float calculate_line_length(int map[map_size][map_size], SDL_FPoint direction);
    void draw(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]);
}