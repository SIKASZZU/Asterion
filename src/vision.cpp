// #include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <set>
#include <algorithm>

#include "offset.hpp"
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
            screen_width, screen_height
        );
        SDL_SetTextureBlendMode(darkness, SDL_BLENDMODE_BLEND);
    }
    void draw_overlay(SDL_Renderer* renderer, struct Offset& offset) {
        // Render darkness to texture
        SDL_SetRenderTarget(renderer, darkness);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
        SDL_RenderClear(renderer);

        auto draw_fade_rect = [&](int gridX, int gridY) {
            SDL_FPoint coords = to_isometric_grid_coordinate(offset, gridX, gridY);
            coords.y -= tile_size / 2;
            SDL_FRect rect = { coords.x, coords.y, tile_size, tile_size };
            float dx = (gridX * tile_size + tile_size / 2) - (player.x);
            float dy = (gridY * tile_size + tile_size / 2) - (player.y);
            float dist = std::sqrt(dx * dx + dy * dy) / (tile_size * 4);
            float t = std::clamp(dist / (render_radius), 0.0f, 1.0f);
            Uint8 alpha = t * 255.0f;
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