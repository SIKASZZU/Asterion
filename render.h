#pragma once
#include "game.h"
#include <iostream>


extern int render_radius;

void draw_nearby(SDL_Renderer* renderer, struct Offset& offset, SDL_Point zero_grid);

void load_render(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player);

void render_map(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player,
    SDL_Texture* tree_tex, SDL_Texture* wall_tex, SDL_Texture* cube_ground_tex);

void render_map_numbers(SDL_Renderer* renderer, const int tile_size, struct Offset& offset, struct Player& player);
