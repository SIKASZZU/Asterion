#pragma once
#include <vector>

#include "game.hpp"
#include "map.hpp"
#include "offset.hpp"

enum class PlayerState {
    Idle,
    Walk,
    Run,
    Jump,
    RunningJump,
    Damage,
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
    SDL_FRect hitboxRect;
    bool collision;
    std::unordered_set<int> collision_array;
    SDL_Point movementVector;  // Will be used to calculate where player will end up at, direction alike.
    SDL_FPoint velocity;
    int animationSpeed;
    bool shifting;  // has player pressed shift?
    bool jumping;  // has player pressed space?
    float z; // vertical offset (pixels)
    float zVelocity; // vertical velocity (pixels per second)
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
    void toggle_collision();
    void update(float deltaTime);
    void create_movementVector(const bool* state);
    void debug(SDL_Renderer* renderer);
    void init();
}
