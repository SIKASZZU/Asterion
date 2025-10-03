#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <utility>
#include <set>

#include "offset.hpp"
#include "game.hpp"

namespace Vision {
    extern SDL_Texture* darkness;

    void create_darkness(SDL_Renderer* renderer);
    void draw_overlay(SDL_Renderer* renderer, struct Offset& offset);
    void update(SDL_Renderer* renderer, struct Offset& offset);
};