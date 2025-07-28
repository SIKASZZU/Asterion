#include "map.hpp"
#include "player.hpp"
#include "raycast.hpp"
#include "isometric_calc.hpp"
#include "game.hpp"
#include "vision.hpp"

#include <iostream>
#include <sdl2/SDL.h>
#include <cmath>

namespace Raycast {
    int half_tile = (tile_size / 2);
    SDL_FPoint source_pos = {};

    void update_source_pos(){
        source_pos = { player.x + player.size, player.y };
    }
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
    float calculate_line_length(int map[map_size][map_size], SDL_FPoint direction) {
        float distance = 0.0f;
        SDL_FPoint start_pos = source_pos;
        bool wall_found = false;

        while (distance < max_ray_length) {
            // fixme: optimise
            if (wall_found == true) {
                // distance koos half_tilega on walli sees, ehk addi v2hem distantsi
                start_pos.x += direction.x * (angle_step);
                start_pos.y += direction.y * (angle_step);
            }
            else {
                start_pos.x += direction.x * (angle_step + half_tile);
                start_pos.y += direction.y * (angle_step + half_tile);
            }
            
            int grid_x = static_cast<int>(start_pos.x / tile_size);
            int grid_y = static_cast<int>(start_pos.y / tile_size);

            if (wall_values.find(map[grid_y][grid_x]) != wall_values.end()) {
                if (!wall_found) { wall_found = true; continue; }

                if (wall_found && wall_values.find(map[grid_y][grid_x]) != wall_values.end()) {
                    // add the wall to visible rect aswell
                    Vision::cutout_rects.insert({grid_x, grid_y});
                    break;
                } 
            }
            distance += angle_step + half_tile;
            Vision::cutout_rects.insert({grid_x, grid_y});
        }
        return distance;
    }
    void draw(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]) {
        if (r_pressed) return;
        Vision::cutout_rects = {};
        // update light source pos
        update_source_pos();
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);

        for (int angle = 0; angle < amount_of_rays; angle += angle_step) {
            SDL_FPoint direction = angle_to_direction(static_cast<float>(angle));
            float calculated_length = calculate_line_length(map, direction);
            SDL_FPoint end = {
                source_pos.x + direction.x * calculated_length,
                source_pos.y + direction.y * calculated_length
            };
            SDL_FPoint iso_start = to_isometric_coordinate(offset, source_pos.x , source_pos.y );
            SDL_FPoint iso_end = to_isometric_coordinate(offset, end.x , end.y );
            SDL_RenderDrawLineF(
                renderer, iso_start.x + half_tile, iso_start.y, iso_end.x + half_tile, iso_end.y
            );
        }
        // std::cout << "Size of walls: " << Vision::cutout_rects.size() << "\n";
    }
}