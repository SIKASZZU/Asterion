#pragma once
#include "game.hpp"
#include "map.hpp"
#include <vector>

static const float DEFAULT_MOVEMENT_SPEED = 30.0f;

struct Player {
    float movement_speed;
    float size;
    float x;
    float y;
    SDL_FRect rect;
    bool collision;
    std::unordered_set<int> collision_array;
    SDL_FPoint movement_vector;  // Will be used to calculate where player will end up at, direction alike.
    SDL_FPoint velocity;
    int animation_speed;
    bool shifting;  // has player pressed shift?
    char last_movement_key;
};

extern Player player;
extern bool collisionX;
extern bool collisionY;
void update_player(int map[map_size][map_size], struct Offset& offset, SDL_Renderer* renderer);