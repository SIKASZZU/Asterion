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
            player.x,
            player.y
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
    // Idea from javidx9 https://www.youtube.com/watch?v=NbSee-XM7WA

    // Current grid cell
    int gridX = static_cast<int>(sourcePos.x / tile_size);
    int gridY = static_cast<int>(sourcePos.y / tile_size);

    // Length of ray from one x or y side to next
    SDL_FPoint rayUnitStep = {
        std::sqrt(1 + (direction.y / direction.x) * (direction.y / direction.x)),
        std::sqrt(1 + (direction.x / direction.y) * (direction.x / direction.y))
    };

    // Step direction (+1 or -1) and initial distances
    SDL_Point step;
    SDL_FPoint rayLength1D;

    if (direction.x < 0) {
        step.x = -1;
        rayLength1D.x = (sourcePos.x - (gridX * tile_size)) / tile_size * rayUnitStep.x;
    } else {
        step.x = 1;
        rayLength1D.x = ((gridX + 1) * tile_size - sourcePos.x) / tile_size * rayUnitStep.x;
    }

    if (direction.y < 0) {
        step.y = -1;
        rayLength1D.y = (sourcePos.y - (gridY * tile_size)) / tile_size * rayUnitStep.y;
    } else {
        step.y = 1;
        rayLength1D.y = ((gridY + 1) * tile_size - sourcePos.y) / tile_size * rayUnitStep.y;
    }

    // Ray walk
    float distance = 0.0f;
    bool wall_found = false;

    while (distance < maxRayLength) {
        // Mark this cell as visible
        auto gP = std::make_pair(gridY, gridX);
        endpointActiveGrids.insert(gP);
        decayGrids.erase(std::remove(decayGrids.begin(), decayGrids.end(), gP),
                         decayGrids.end());

        // Step in whichever direction is closer
        if (rayLength1D.x < rayLength1D.y) {
            gridX += step.x;
            distance = rayLength1D.x * tile_size;
            rayLength1D.x += rayUnitStep.x;
        } else {
            gridY += step.y;
            distance = rayLength1D.y * tile_size;
            rayLength1D.y += rayUnitStep.y;
        }

        // Bounds check
        if (gridX < 0 || gridX >= map_size || gridY < 0 || gridY >= map_size)
            break;

        // Hit wall?
        if (wall_values.find(map[gridY][gridX]) != wall_values.end()) {
            wall_found = true;
            endpointActiveGrids.insert(std::make_pair(gridY, gridX));
            break;
        }
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