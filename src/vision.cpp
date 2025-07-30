#include <SDL3/SDL.h>
#include <vector>

#include "game.hpp"
#include "vision.hpp"
#include "isometric_calc.hpp"
#include "raycast.hpp"

namespace Vision {
    SDL_Texture* darkness   = nullptr;
    SDL_Texture* lightMask  = nullptr;
    float alpha = 0;
    // Custom DST_OUT blend = (0, 1-srcA, add) for both color & alpha
    static SDL_BlendMode DST_OUT_BLEND() {
        return SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ZERO,                // srcColorFactor
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, // dstColorFactor
            SDL_BLENDOPERATION_ADD,              // colorOp
            SDL_BLENDFACTOR_ZERO,                // srcAlphaFactor
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, // dstAlphaFactor
            SDL_BLENDOPERATION_ADD               // alphaOp
        );
    }
    void create_vision_textures(SDL_Renderer* renderer) {
        // Darkness texture (opaque black)
        darkness = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            screen_width, screen_height
        );
        SDL_SetTextureBlendMode(darkness, SDL_BLENDMODE_BLEND);

        // Light‑mask texture (alpha mask)
        lightMask = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            screen_width, screen_height
        );
        SDL_SetTextureBlendMode(lightMask, SDL_BLENDMODE_BLEND);
    }

    // Draw your polygon filled solid white into whichever target is bound
    static void draw_polygon_mask(SDL_Renderer* renderer, struct Offset& offset) {
        const auto& pts = Raycast::get_ray_endpoints();
        if (pts.size() < 3) return;

        SDL_FPoint center = Raycast::get_source_pos(offset);
        std::vector<SDL_Vertex> verts;
        std::vector<int> idx;

        // Center vertex, white opaque
        verts.push_back(SDL_Vertex{
            .position = { center.x + (tile_size / 2), center.y  },
            .color    = { 255, 255, 255, 0.5f },  // todo: replace 0.5f with alpha // was testing
            .tex_coord= { 0, 0 }
        });

        // Outer verts
        for (auto& p : pts) {
            verts.push_back(SDL_Vertex{
                .position = { p.x + (tile_size / 2), p.y },
                .color    = { 255, 255, 255, alpha },
                .tex_coord= { 0, 0 }
            });
        }

        // Build triangle‐fan indices
        for (int i = 1; i < (int)pts.size(); ++i) {
            idx.push_back(0);
            idx.push_back(i);
            idx.push_back(i + 1);
        }
        // close the fan
        idx.push_back(0);
        idx.push_back((int)pts.size());
        idx.push_back(1);

        // We can use blend none since verts are fully opaque
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_RenderGeometry(renderer,
                           nullptr,
                           verts.data(), (int)verts.size(),
                           idx.data(), (int)idx.size());
    }
    void update(SDL_Renderer* renderer, struct Offset& offset) {
        if (r_pressed) return;

        // 1) Ensure textures exist
        if (!darkness || !lightMask) {
            create_vision_textures(renderer);
        }

        // 2) Build the light‐mask: transparent everywhere except polygon
        SDL_SetRenderTarget(renderer, lightMask);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
        SDL_RenderClear(renderer);
        draw_polygon_mask(renderer, offset);

        // 3) Build the darkness: solid black
        SDL_SetRenderTarget(renderer, darkness);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);  // you can tweak alpha here
        SDL_RenderClear(renderer);

        // 4) Subtract the mask: DST_OUT removes darkness where mask α=255
        SDL_SetRenderDrawBlendMode(renderer, DST_OUT_BLEND());
        SDL_RenderTexture(renderer, lightMask, nullptr, nullptr);

        // 5) Back to screen and draw combined result
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderTexture(renderer, darkness, nullptr, nullptr);
    }
}
