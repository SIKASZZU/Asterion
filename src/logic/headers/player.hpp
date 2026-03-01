#pragma once
#include <vector>

#include "game.hpp"
#include "map.hpp"
#include "offset.hpp"

enum class PlayerState {
    Idle,
    Walk,
    Run
};

// PlayerState state;

struct PlayerData {
    PlayerState state;
    float movementSpeed;
    float size;
    SDL_Point grid;
    float x;
    float y;
    SDL_FRect rect;
    bool collision;
    std::unordered_set<int> collision_array;
    SDL_FPoint movementVector;  // Will be used to calculate where player will end up at, direction alike.
    SDL_FPoint velocity;
    int animationSpeed;
    bool shifting;  // has player pressed shift?
    char lastMovementKey;
    bool cartesianMovement;
};
extern PlayerData player;

namespace PlayerNS {
    extern float tilesPerSecond;
    extern float defaultMovementSpeed;
    extern float defaultMovementSpeedShifting;
    extern bool collisionX;
    extern bool collisionY;
    void update(int map[mapSize][mapSize], struct Offset& offset, SDL_Renderer* renderer, float deltaTime);
    void create_movementVector(const bool* state);
}
