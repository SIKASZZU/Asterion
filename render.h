#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "game.h"
#include "textures.h"

void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

void render_map_numbers(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

extern std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets;

