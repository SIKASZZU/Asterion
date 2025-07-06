#pragma once
#include <iostream>
#include "game.h"
#include "textures.h"

extern int render_radius;

void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player, const std::vector<SDL_Texture>& texture_vector);

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player, const std::vector<SDL_Texture>& texture_vector);

void render_map_numbers(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);
