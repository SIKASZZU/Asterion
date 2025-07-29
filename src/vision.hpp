#pragma once
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include "game.hpp"
#include <utility>
#include <set>


namespace Vision {
    extern SDL_Texture* darkness;
    extern SDL_Texture* light;
    extern float alpha;

    void create_vision_textures(SDL_Renderer* renderer);
    static void draw_polygon_mask(SDL_Renderer* renderer, struct Offset& offset);
    void update(SDL_Renderer* renderer, struct Offset& offset);
};

