
// #include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <utility>
#include <set>

#include "game.hpp"
#include "vision.hpp"
#include "isometric_calc.hpp"


namespace Vision {
    SDL_Texture* darkness = nullptr;
    std::set<std::pair<int,int>> cutout_rects = {};

    void create_darkness(SDL_Renderer* renderer) {
        darkness = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        screen_width, screen_height
        );
        SDL_SetTextureBlendMode(darkness, SDL_BLENDMODE_BLEND);
    }

    void draw_overlay(SDL_Renderer* renderer, struct Offset& offset) {
        if (r_pressed) return;
        // Render darkness to texture
        SDL_SetRenderTarget(renderer, darkness);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderClear(renderer);

        // Cut out transparent holes
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        for (const auto& [x, y] : cutout_rects) {
            SDL_FPoint coords = to_isometric_grid_coordinate(offset, x, y);
            SDL_Point intCoords = { static_cast<int>(coords.x), static_cast<int>(coords.y)};

            intCoords.y -= tile_size / 2;
            SDL_Rect rect = { intCoords.x, intCoords.y, tile_size, tile_size };
            SDL_RenderFillRect(renderer, &rect);
        }
        // Reset and render to screen
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, darkness, nullptr, nullptr);
    }
}