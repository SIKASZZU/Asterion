#include "map.hpp"
#include "player.hpp"
#include "raycast.hpp"
#include "isometric_calc.hpp"
#include "game.hpp"

#include <iostream>
#include <sdl2/SDL.h>
#include <cmath>

namespace Raycast {
    int ray_length = 100;

    float to_radians(float degrees) {
        return degrees * (PI / 180.0f);
    }

    // Computes direction vector from an angle
    SDL_FPoint angle_to_direction(float angle_deg) {
        float angle_rad = to_radians(angle_deg);
        return {
            cosf(angle_rad),
            sinf(angle_rad)
        };
    }
    float calculate_line_length(SDL_FPoint direction) {
        float length;
        SDL_FPoint starting_pos = { player.x, player.y };

        return length;
    }

    void draw(SDL_Renderer* renderer, struct Offset& offset) {
        // Cornflower blue rays
        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);

        for (int angle = 0; angle < 360; angle += step) {
            SDL_FPoint dir = angle_to_direction(static_cast<float>(angle));
            SDL_FPoint start = { 
                player.x + player.size, 
                player.y + player.size
            };
            SDL_FPoint end = {
                start.x + dir.x * ray_length,
                start.y + dir.y * ray_length
            };

            SDL_FPoint iso_start = to_isometric_grid_coordinate(offset, start.x / tile_size, start.y / tile_size);
            SDL_FPoint iso_end = to_isometric_grid_coordinate(offset, end.x / tile_size, end.y / tile_size);
            SDL_RenderDrawLineF(renderer,
                iso_start.x,
                iso_start.y,
                iso_end.x  ,
                iso_end.y  
            );
        }
    }
}