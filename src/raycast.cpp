#include "map.hpp"
#include "player.hpp"
#include "raycast.hpp"
#include "isometric_calc.hpp"
#include "game.hpp"
#include "vision.hpp"

#include <SDL3/SDL.h>
#include <iostream>
#include <set>
#include <deque>
#include <utility>
#include <cmath>
#include <algorithm>

namespace Raycast {
    SDL_FPoint sourcePos = {};
    bool showRays = true;
    signed int maxActiveSize = ((render_radius / tile_size) * (render_radius / tile_size) * PI);
    signed int maxDecaySize = maxActiveSize / 2;
    float maxRayLength = render_radius * (tile_size * 0.75);
    bool updateMaxGridSize = true;
    /// endpointActiveGrids.size() >= maxActiveSize
    std::set<std::pair<int, int>> endpointActiveGrids;
    // CONSTEXPRES activeGridsMaxSize.size() == maxActiveSize
    std::set<std::pair<int, int>> activeGridsMaxSize;
    std::deque<std::pair<int, int>> decayGrids;

    void update_sourcePos() {
        sourcePos = {
            player.x + (player.size / 2),
            player.y + (player.size / 2)
        };
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
        SDL_FPoint start_pos = sourcePos;
        bool wall_found = false;

        while (distance < maxRayLength) {
            // fixme: optimise kui grid on hit, ss arvuta j2rgmise gridi oma
            if (wall_found == true) {
                // liiga suure stepiga on wall ehk otsi increment
                start_pos.x += direction.x * (increment);
                start_pos.y += direction.y * (increment);
            }
            else {
                start_pos.x += direction.x * (increment + (tile_size / 2));
                start_pos.y += direction.y * (increment + (tile_size / 2));
            }
            int grid_x = static_cast<int>(start_pos.x / tile_size);
            int grid_y = static_cast<int>(start_pos.y / tile_size);
            auto gP = std::make_pair(grid_x, grid_y);
            if (wall_values.find(map[grid_y][grid_x]) != wall_values.end()) {
                if (!wall_found) { wall_found = true; continue; }
                if (wall_found && wall_values.find(map[grid_y][grid_x]) != wall_values.end()) {
                    // add the wall to visible rect aswell
                    endpointActiveGrids.insert(gP);
                    break;
                }
            }
            distance += increment + (tile_size / 2);
            endpointActiveGrids.insert(gP);
            // todo: SIIN KUKUB PERFORMANCE!!! optimiseerimisest v6ida tagasi?
            // Removes duplicates aswell
            decayGrids.erase(std::remove(decayGrids.begin(), decayGrids.end(), gP),
                decayGrids.end()
            );
        }
        return distance;
    }
    void calculate_active_grids(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]) {
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);
        for (int angle = 0; angle < 360; angle += angleStep) {
            SDL_FPoint direction = angle_to_direction(static_cast<float>(angle));
            float calculated_length = calculate_line_length(map, direction);
            if (showRays == true) {
                SDL_FPoint end = {
                    sourcePos.x + direction.x * calculated_length,
                    sourcePos.y + direction.y * calculated_length
                };
                SDL_FPoint iso_start = to_isometric_coordinate(offset, sourcePos.x, sourcePos.y);
                SDL_FPoint iso_end = to_isometric_coordinate(offset, end.x, end.y);
                SDL_RenderLine(
                    renderer, iso_start.x + (tile_size / 2), iso_start.y, iso_end.x + (tile_size / 2), iso_end.y
                );
            }
        }
    }
    void calculate_decay_grids() {
        // todo: decay grids setist ei removei mingeid random v22rtusi 2ra, just mazeis olles
        // Testi sedasi, et visionis renderi valged "active" rectid enne "decay" recte ning ss mine mazei.
        // hetkel ongi nii, et renderib decay ss active ehk active overwritib. Fix the overlap!
        std::set_difference(
            activeGridsMaxSize.begin(), activeGridsMaxSize.end(),
            endpointActiveGrids.begin(), endpointActiveGrids.end(),
            std::inserter(decayGrids, decayGrids.begin())
        );
        activeGridsMaxSize = endpointActiveGrids;
    }
    void clear_arrays() {
        endpointActiveGrids.clear();
        while (decayGrids.size() >= maxDecaySize) {
            decayGrids.pop_back();
        }
    }
    void update_max_grid_size() {
        if (!updateMaxGridSize) return;
        updateMaxGridSize = false;
        maxActiveSize = (render_radius * render_radius * PI);
        maxDecaySize = maxActiveSize;
    }
    void update(SDL_Renderer* renderer, struct Offset& offset, int map[map_size][map_size]) {
        if (!r_pressed) return;
        // update light source pos
        update_sourcePos();
        update_max_grid_size();
        clear_arrays();
        calculate_active_grids(renderer, offset, map);
        calculate_decay_grids();
    }
}