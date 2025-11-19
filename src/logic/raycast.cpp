#include "map.hpp"
#include "player.hpp"
#include "raycast.hpp"
#include "isometric_calc.hpp"
#include "game.hpp"
#include "vision.hpp"
#include "offset.hpp"

#include <SDL3/SDL.h>
#include <iostream>
#include <set>
#include <deque>
#include <utility>
#include <cmath>
#include <algorithm>

namespace Raycast {
    SDL_FPoint sourcePos = {};
    bool showRays = false;
    signed int maxActiveSize = ((renderRadius / tileSize) * (renderRadius / tileSize) * PI);
    signed int maxDecaySize = maxActiveSize / 2;
    float maxRayLength = renderRadius * (tileSize * 0.75);
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
    SDL_FPoint angle_to_direction(float angleDeg) {
        float angleRad = to_radians(angleDeg);
        return {
            cosf(angleRad),
            sinf(angleRad)
        };
    }
    float calculate_line_length(int map[mapSize][mapSize], SDL_FPoint direction) {
        // Idea from javidx9 https://www.youtube.com/watch?v=NbSee-XM7WA

        // Current grid cell
        int gridX = static_cast<int>(sourcePos.x / tileSize);
        int gridY = static_cast<int>(sourcePos.y / tileSize);

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
            rayLength1D.x = (sourcePos.x - (gridX * tileSize)) / tileSize * rayUnitStep.x;
        }
        else {
            step.x = 1;
            rayLength1D.x = ((gridX + 1) * tileSize - sourcePos.x) / tileSize * rayUnitStep.x;
        }

        if (direction.y < 0) {
            step.y = -1;
            rayLength1D.y = (sourcePos.y - (gridY * tileSize)) / tileSize * rayUnitStep.y;
        }
        else {
            step.y = 1;
            rayLength1D.y = ((gridY + 1) * tileSize - sourcePos.y) / tileSize * rayUnitStep.y;
        }

        // Ray walk // borderline fucked
        float distance = 0.0f;
        while (distance < maxRayLength) {
            endpointActiveGrids.insert(std::make_pair(gridY, gridX));
            if (rayLength1D.x < rayLength1D.y) {
                gridX += step.x;
                distance = rayLength1D.x * tileSize;
                rayLength1D.x += rayUnitStep.x;
            }
            else {
                gridY += step.y;
                distance = rayLength1D.y * tileSize;
                rayLength1D.y += rayUnitStep.y;
            }
            // Hit wall?
            if (wallValues.find(map[gridY][gridX]) != wallValues.end()) {
                endpointActiveGrids.insert(std::make_pair(gridY, gridX));
                break;
            }
        }
        return distance;
    }
    void calculate_active_grids(SDL_Renderer* renderer, struct Offset& offset, int map[mapSize][mapSize]) {
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);
        for (int angle = 0; angle < 360; angle += angleStep) {
            SDL_FPoint direction = angle_to_direction(static_cast<float>(angle));
            float calculated_length = calculate_line_length(map, direction);
            if (showRays == true) {
                SDL_FPoint end = {
                    sourcePos.x + direction.x * calculated_length,
                    sourcePos.y + direction.y * calculated_length
                };
                SDL_FPoint isoStart = to_isometric_coordinate(sourcePos.x, sourcePos.y);
                SDL_FPoint isoEnd = to_isometric_coordinate(end.x, end.y);
                SDL_RenderLine(
                    renderer, isoStart.x + (tileSize / 2), isoStart.y, isoEnd.x + (tileSize / 2), isoEnd.y
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
        maxActiveSize = (renderRadius * renderRadius * PI);
        maxDecaySize = maxActiveSize / 2;
    }
    void update(SDL_Renderer* renderer, struct Offset& offset, int map[mapSize][mapSize]) {
        if (!r_pressed) return;
        // update light source pos
        update_sourcePos();
        update_max_grid_size();
        clear_arrays();
        calculate_active_grids(renderer, offset, map);
        calculate_decay_grids();
    }
}