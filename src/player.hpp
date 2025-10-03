#pragma once
#include <vector>

#include "game.hpp"
#include "map.hpp"
#include "offset.hpp"

struct PlayerData {
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
extern PlayerData player;

namespace PlayerNS {
    extern float tilesPerSecond;
    extern float DEFAULT_MOVEMENT_SPEED;
    extern bool collisionX;
    extern bool collisionY;
    void update(int map[map_size][map_size], struct Offset& offset, SDL_Renderer* renderer, float deltaTime);
}
