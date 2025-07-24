#pragma once
#include "game.hpp"
#include "map.hpp"
#include <vector>

static const float DEFAULT_PLAYER_MOVEMENT_SPEED = 50.0f;

struct Player {
    float movement_speed;
    int size;
    float x;
    float y;
    SDL_FRect rect;
    bool collision;
    std::vector<int> collision_array;
    SDL_FPoint movement_vector;  // Will be used to calculate where player will end up at, direction alike.
    int animation_speed;
    bool shifting;  // has player pressed shift?
};

extern Player player;

void update_player(int map[map_size][map_size], struct Offset& offset, const Uint8* state, SDL_Renderer* renderer);