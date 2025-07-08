#pragma once
#include <iostream>
#include <unordered_map>

#include "game.h"
#include "textures.h"

extern int render_radius;

void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player, const std::vector<SDL_Texture>& texture_vector);

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player, const std::vector<SDL_Texture>& texture_vector);

void render_map_numbers(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);


struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second << 1);
    }
};

extern std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets;

