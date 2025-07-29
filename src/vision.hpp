#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include "game.hpp"
#include <utility>
#include <set>


namespace Vision {
    extern SDL_Texture* darkness;
    extern std::set<std::pair<int, int>> cutout_rects;

    void create_darkness(SDL_Renderer* renderer);
    void draw_overlay(SDL_Renderer* renderer, struct Offset& offset);
};

