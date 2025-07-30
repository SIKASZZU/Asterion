#include "map.hpp"
#include "player.hpp"
#include "raycast.hpp"
#include "isometric_calc.hpp"
#include "game.hpp"
#include "vision.hpp"

#include <iostream>
#include <SDL3/SDL.h>
#include <cmath>

namespace Raycast {
    int half_tile = (tile_size / 2);
    std::vector<SDL_FPoint> ray_endpoints = {};
    SDL_FPoint source_pos = {};
    bool draw_rays = false;
    const int increment = 2;  // min 1
    int last_ray_length = 0;

    std::vector<SDL_FPoint>& get_ray_endpoints() {
        return ray_endpoints;
    }
    SDL_FPoint get_source_pos(struct Offset& offset) {
        return to_isometric_coordinate(offset, source_pos.x, source_pos.y);
    }
    void update_source_pos() {
        source_pos = { player.x, player.y };
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
    float calculate_line_length(int map[map_size][map_size], struct Offset& offset, SDL_FPoint direction, SDL_Renderer* renderer) {
        float distance = 0.0f;
        SDL_FPoint start_pos = source_pos;
        bool wall_found = false;

        // alati saab alguses vaadata v2hemalt half_tile kaugusele.
        start_pos.x += half_tile;
        start_pos.y += half_tile;

        while (distance < max_ray_length) {
            // fixme: optimise
            if (wall_found == true) {
                // distance koos half_tilega/2 on walli sees, ehk addi v2hem distantsi
                start_pos.x += direction.x * (increment);
                start_pos.y += direction.y * (increment);
            }
            else {
                start_pos.x += direction.x * (increment + last_ray_length); //  + (half_tile / 2)
                start_pos.y += direction.y * (increment + last_ray_length); //  + (half_tile / 2)
            }
            int grid_x = static_cast<int>(start_pos.x / tile_size);
            int grid_y = static_cast<int>(start_pos.y / tile_size);
            if (wall_values.find(map[grid_y][grid_x]) != wall_values.end()) {
                if (!wall_found) {
                    wall_found = true;
                    continue;
                }
                if (wall_found && wall_values.find(map[grid_y][grid_x]) != wall_values.end()) {
                    break;
                }
            }
            distance += increment + last_ray_length; // + (half_tile / 2)
        }
        last_ray_length = distance ? max_ray_length != distance : last_ray_length = 0;
        return distance;
    }
    void calculate(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]) {
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);
        ray_endpoints.clear();  // clear at the start of calculate()
        for (int angle = 0; angle < amount_of_rays; angle += angle_step) {
            SDL_FPoint direction = angle_to_direction(static_cast<float>(angle));
            float calculated_length = calculate_line_length(map, offset, direction, renderer);

            SDL_FPoint end = {
                source_pos.x + direction.x * calculated_length,
                source_pos.y + direction.y * calculated_length
            };
            SDL_FPoint iso_end = to_isometric_coordinate(offset, end.x, end.y);
            ray_endpoints.push_back(iso_end); // Save the point for later polygon use

            if (draw_rays) {
                SDL_FPoint iso_start = to_isometric_coordinate(offset, source_pos.x, source_pos.y);
                SDL_RenderLine(
                    renderer, iso_start.x + half_tile, iso_start.y,
                    iso_end.x + half_tile, iso_end.y
                );
            }
        }
    }
    void update(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]) {
        if (r_pressed) return;
        // update light source pos
        update_source_pos();
        calculate(renderer, offset, map);
    }
}