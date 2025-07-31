// #include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <set>

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
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderClear(renderer);

        // Cut out transparent holes
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        std::cout << "Raycast::ray_endpoints_grids.size(): " << Raycast::ray_endpoints_grids.size() << "\n";
        for (const auto& [x, y] : Raycast::ray_endpoints_grids) {
            SDL_FPoint coords = to_isometric_grid_coordinate(offset, x, y);
            // coords.y -= tile_size / 2;
            SDL_FRect rect = { coords.x, coords.y, tile_size, tile_size };
            SDL_RenderFillRect(renderer, &rect);
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