#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <utility>
#include <set>

#include "offset.hpp"
#include "game.hpp"

namespace Vision {
    extern SDL_Texture* darkness;
    extern bool enabled;

    void create_darkness(GameState* gS);
    void draw_overlay(SDL_Renderer* renderer);
    void update(SDL_Renderer* renderer);
};