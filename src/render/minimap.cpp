#include <SDL3/SDL.h>
#include <algorithm>

#include "game.hpp"
#include "map.hpp"
#include "player.hpp"
#include "minimap.hpp"
#include "raycast.hpp"

namespace Minimap {
    // Draw a simple top-right minimap of size 200x200px showing nearby tiles.
    void render_minimap(SDL_Renderer* renderer) {
        const float minimapSize = 200.0f;
        const float margin = 10.0f;
        const float x0 = screenWidth - minimapSize - margin;
        const float y0 = margin;

        SDL_FRect bg = { x0, y0, minimapSize, minimapSize };
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        // SDL_RenderFillRect(renderer, &bg);

        int tiles = std::max(1, renderRadius * 2 + 1);
        float cell = minimapSize / tiles;

        int startX = player.gridX - renderRadius;
        int startY = player.gridY - renderRadius;

        bool haveEndpoints = !Raycast::endpointActiveGrids.empty();
        bool raycastEnabled = Raycast::enabled;
        for (int ry = 0; ry < tiles; ++ry) {
            for (int rx = 0; rx < tiles; ++rx) {
                int gx = startX + rx;
                int gy = startY + ry;
                if (gx < 0 || gy < 0 || gx >= mapSize || gy >= mapSize) continue;
                SDL_FRect cellRect = { bg.x + rx * cell, bg.y + ry * cell, cell, cell };

                int gridValue = map[gy][gx];
                if (haveEndpoints && raycastEnabled) {
                    // only render tiles that are in the visible endpoint set
                    if (Raycast::endpointActiveGrids.find({gy, gx}) == Raycast::endpointActiveGrids.end()) {
                        if (std::find(Raycast::decayGrids.begin(), Raycast::decayGrids.end(), std::make_pair(gy, gx)) == Raycast::decayGrids.end()) {

                            // unseen / out of vision
                            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
                            SDL_RenderFillRect(renderer, &cellRect);
                            continue;
                        }
                    }
                }

                if (wallValues.find(gridValue) != wallValues.end()) {
                    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255); // walls
                }
                else if (gridValue == Map::TREE || gridValue == Map::TREE_TRUNK) {
                    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); // trees
                }
                else if (gridValue == Map::MAZE_GROUND_CUBE || gridValue == Map::GROUND_CUBE || gridValue == Map::MAZE_GROUND_SPRITE) {
                    SDL_SetRenderDrawColor(renderer, 200, 200, 150, 255); // ground
                }
                else if (gridValue == Map::VOID_CUBE || gridValue == Map::VOID_CUBE_NEIGHBOUR) {
                    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // void
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255); // default
                }
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }

        // Draw player marker in center
        float px = bg.x + (renderRadius)*cell + cell * 0.5f;
        float py = bg.y + (renderRadius)*cell + cell * 0.5f;
        SDL_FRect pRect = { px - cell * 0.35f, py - cell * 0.35f, cell * 0.7f, cell * 0.7f };
        SDL_SetRenderDrawColor(renderer, 220, 40, 40, 255);
        SDL_RenderFillRect(renderer, &pRect);

        // Border
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        SDL_RenderRect(renderer, &bg);
    }
}

// Expose C-linkage function matching header
void render_minimap(SDL_Renderer* renderer) {
    Minimap::render_minimap(renderer);
}
