// #include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <set>
#include <algorithm>

#include "offset.hpp"
#include "player.hpp"
#include "game.hpp"
#include "vision.hpp"
#include "isometric_calc.hpp"
#include "raycast.hpp"

namespace Vision {
    SDL_Texture* darkness = nullptr;

    void create_darkness(SDL_Renderer* renderer) {
        darkness = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            screenWidth, screenHeight
        );
        SDL_SetTextureBlendMode(darkness, SDL_BLENDMODE_BLEND);
    }
    void draw_overlay(SDL_Renderer* renderer, struct Offset& offset) {
        // Render darkness to texture
        SDL_SetRenderTarget(renderer, darkness);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        // Compute day/night brightness through central helper: 1.0 = day, 0.0 = night
        float brightness = dayNightEnabled ? get_day_brightness() : 1.0f;
        float dayFactor = 1.0f - brightness; // 0 at day, 1 at night
        Uint8 baseClearAlpha = static_cast<Uint8>(120.0f * dayFactor);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, baseClearAlpha);
        SDL_RenderClear(renderer);

        auto draw_fade_rect = [&](int gridX, int gridY) {
            SDL_FPoint coords = to_isometric_grid_coordinate(gridX, gridY);
            coords.y -= tileSize / 2;
            SDL_FRect rect = { coords.x, coords.y, tileSize, tileSize };
            float dx = (gridX * tileSize + tileSize / 2) - (player.x);
            float dy = (gridY * tileSize + tileSize / 2) - (player.y);
            float dist = std::sqrt(dx * dx + dy * dy) / (tileSize * 4);
            float t = std::clamp(dist / (renderRadius), 0.0f, 1.0f);
            // tile alpha scaled by dayFactor so at day it's transparent
            Uint8 alpha = static_cast<Uint8>(t * 255.0f * dayFactor);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_RenderFillRect(renderer, &rect);
            };

        // Draw decay grids (darker, farther away)
        for (const auto& [y, x] : Raycast::decayGrids) {
            // fixme: optimize this shit. o(n)
            // problem: overlap within decayGrids with enpointActiveGrids
            if (Raycast::endpointActiveGrids.count(std::make_pair(y, x)) > 0) {
                continue; // skip, it became active again
            }
            draw_fade_rect(x, y);
        }

        // Draw active grids (lighter, close to player)
        for (const auto& [y, x] : Raycast::endpointActiveGrids) {
            draw_fade_rect(x, y);
        }

        // Reset and render to screen
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderTexture(renderer, darkness, nullptr, nullptr);
    }
    void update(SDL_Renderer* renderer, struct Offset& offset) {
        if (!v_pressed) return;
        draw_overlay(renderer, offset);
    }
}