#include <cmath>
#include <iostream>
#include <algorithm>

#include "render.hpp"
#include "map.hpp"
#include "game.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"
#include "collision.hpp"
#include "offset.hpp"

const SDL_Point spawnpointGrid = { x: mapSize / 2, y : mapSize / 2 };
bool groundOffsetAdded = false;
int groundOffsetAmount = 0;

// Suggested constants (adjust to taste)
const float MAX_SPEED = 4000.0f; // normal value 400.0f
const float MAX_WALK_SPEED = MAX_SPEED / 4;
const float ACCEL = 2000.0f;    // High value for snappy feel
const float FRICTION = 1500.0f; // How fast player slides to a stop
const float GRAVITY = 2000.0f; // Gravity for jump (pixels/s^2)
const float JUMP_VELOCITY = 350.0f; // Initial jump velocity (pixels/s)

PlayerData player = {
    state: PlayerState::Idle,
    movementSpeed : PlayerNS::defaultMovementSpeed,
    size : (tileSize),
    grid : (spawnpointGrid.x, spawnpointGrid.y),
    x : static_cast<float>(spawnpointGrid.x * tileSize),
    y : static_cast<float>(spawnpointGrid.y * tileSize),
    rect : {0.0, 0.0, 0.0, 0.0},
    collision : false,
    collision_array : wallValues,
    movementVector : {0, 0},
    velocity : {0, 0},
    animationSpeed : 1,
    shifting : false,
    jumping : false,
    z : 0.0f,
    zVelocity : 0.0f,
    lastMovementKey : 'a',
    cartesianMovement : false,
};

const char* stateToString(PlayerState s) {
    switch (s) {
    case (PlayerState::Idle): return "Idle"; break;
    case (PlayerState::Walk): return "Walk"; break;
    case (PlayerState::Run): return "Run"; break;
    case (PlayerState::Jump): return "Jump"; break;
    case (PlayerState::RunningJump): return "RunningJump"; break;
    default: return "Unconfigured state"; break;
    }
}

namespace PlayerNS {
    float tilesPerSecond = 2.0f;
    float defaultMovementSpeed = tileSize * tilesPerSecond;
    float defaultMovementSpeedShifting = defaultMovementSpeed / 4;

    void create_movementVector(const bool* state) {
        SDL_Point dir = { 0, 0 };
        // player.movementVector = { 0, 0 }

        if (state[SDL_SCANCODE_W]) {
            dir.y = -1;
            player.lastMovementKey = 'w';
            if (player.cartesianMovement == true) {
                dir.x = -1;
            }
        }
        if (state[SDL_SCANCODE_S]) {
            dir.y = 1;
            player.lastMovementKey = 's';
            if (player.cartesianMovement == true) {
                dir.x = 1;
            }
        }
        if (state[SDL_SCANCODE_A]) {
            dir.x = -1;
            player.lastMovementKey = 'a';
            if (player.cartesianMovement == true) {
                dir.y = 1;
            }
        }
        if (state[SDL_SCANCODE_D]) {
            dir.x = 1;
            player.lastMovementKey = 'd';
            if (player.cartesianMovement == true) {
                dir.y = -1;
            }
        }
        if (player.cartesianMovement == true) {
            if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A]
                && !state[SDL_SCANCODE_S] && !state[SDL_SCANCODE_W]) {
                dir.x = 0;
                dir.y = 0;
            }
        }
        else {
            if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A]) { dir.x = 0; }
            if (!state[SDL_SCANCODE_S] && !state[SDL_SCANCODE_W]) { dir.y = 0; }
        }
        player.movementVector = { dir.x, dir.y };
    }

    SDL_FPoint validate_velocity(SDL_FPoint newVelocity) {
        SDL_FRect templatePlayerRect = {
            player.x,
            player.y,
            // see / 2 on h2kk, sest player size on nyyd full tileSize. COllision arvutamise m6ttes, tegin /2
            player.rect.w / 2,
            player.rect.h / 2
        };

        SDL_FRect rectX = templatePlayerRect;
        rectX.x += newVelocity.x;

        SDL_FRect rectY = templatePlayerRect;
        rectY.y += newVelocity.y;

        if (check_collision(map, player, rectX)) {
            newVelocity.x = 0;
        }
        if (check_collision(map, player, rectY)) {
            newVelocity.y = 0;
        }
        return newVelocity;
    }

    void update_rect() {

        // apply ground offset to player.y
        auto it = randomOffsetsGround.find(make_grid_key(player.grid.y, player.grid.x));
        if (it != randomOffsetsGround.end()) {
            groundOffsetAmount = it->second / 2;
        }
        else {
            groundOffsetAmount = 0;
        }

        float x = player.x + player.size / 4.0f;
        float y = player.y - player.size / 4.0f;
        SDL_FPoint coords = to_isometric_coordinate(x, y);
        player.rect = {
            coords.x - (player.size / 4) + offset.x,
            coords.y - (player.size / 4) + offset.y - groundOffsetAmount - player.z,
            player.size,
            player.size
        };
    }

    void calculate_new_coordinates(float deltaTime) {
        SDL_Point dir = player.movementVector;

        float currentMaxSpeed = player.shifting ? MAX_WALK_SPEED : MAX_SPEED;

        // velocity calculations

        if (dir.x != 0) {
            player.velocity.x += dir.x * ACCEL * deltaTime;
        }
        else {
            if (player.velocity.x > 0)
                player.velocity.x = std::max(0.0f, player.velocity.x - FRICTION * deltaTime);
            else if (player.velocity.x < 0)
                player.velocity.x = std::min(0.0f, player.velocity.x + FRICTION * deltaTime);
        }

        if (dir.y != 0) {
            player.velocity.y += dir.y * ACCEL * deltaTime;
        }
        else {
            if (player.velocity.y > 0)
                player.velocity.y = std::max(0.0f, player.velocity.y - FRICTION * deltaTime);
            else if (player.velocity.y < 0)
                player.velocity.y = std::min(0.0f, player.velocity.y + FRICTION * deltaTime);
        }

        player.velocity.x = std::clamp(player.velocity.x, -currentMaxSpeed, currentMaxSpeed);
        player.velocity.y = std::clamp(player.velocity.y, -currentMaxSpeed, currentMaxSpeed);

        player.movementSpeed = (sqrt(pow(player.velocity.x, 2) + pow(player.velocity.y, 2)));

        SDL_FPoint finalVel = validate_velocity({ player.velocity.x * deltaTime, player.velocity.y * deltaTime });

        player.x += finalVel.x;
        player.y += finalVel.y;

        player.grid = {
            static_cast<int>((player.x + player.size / 2.0f) / tileSize),
            static_cast<int>((player.y + player.size / 2.0f) / tileSize)
        };

        // Jump physics (vertical axis)
        // If jump was requested and player is on ground, give an initial upward velocity
        if (player.jumping && player.z == 0.0f) {
            player.zVelocity = JUMP_VELOCITY;
        }

        // Integrate vertical motion
        if (player.z > 0.0f || player.zVelocity > 0.0f || player.jumping) {
            player.z += player.zVelocity * deltaTime;
            player.zVelocity -= GRAVITY * deltaTime;

            if (player.z <= 0.0f) {
                player.z = 0.0f;
                player.zVelocity = 0.0f;
                player.jumping = false; // landed
            }
        }
    }

    void set_state() {
        // TODO: idle needs to be the default return but player.movementVector doesnt get reset for some reason.
        // if (player.velocity.x == 0 && player.velocity.y == 0 && !player.jumping) {
        //     player.state = PlayerState::Idle;
        //     return;
        // }
        if (player.jumping && player.state != PlayerState::Run && player.state != PlayerState::RunningJump) {
            player.state = PlayerState::Jump;
            return;
        }
        else if (player.jumping) {
            player.state = PlayerState::RunningJump;
            return;
        }
        if (player.shifting && player.movementSpeed != 0) {
            player.state = PlayerState::Walk;
            return;
        }
        else if (!player.shifting && player.movementSpeed != 0) {
            player.state = PlayerState::Run;
            return;
        }
        player.state = PlayerState::Idle;
        return;
    }

    void update_animation_speed() {

        // todo: animation speed has to be equal to length of an action. Jumping, RunningJump is fixed time.
        switch (player.state) {
        case (PlayerState::Idle): {
            player.animationSpeed = 120;
            break;
        }
        case (PlayerState::Walk): {
            player.animationSpeed = tileSize * 1.4f;
            break;
        }
        case (PlayerState::Run): {
            player.animationSpeed = tileSize * 1.2f;
            break;
        }
        case (PlayerState::Jump): {
            player.animationSpeed = 20;
            break;
        }
        case (PlayerState::RunningJump): {
            player.animationSpeed = 65;
            break;
        }
        }
    }

    void update(int map[mapSize][mapSize], struct ::Offset& offset, SDL_Renderer* renderer, float deltaTime) {
        calculate_new_coordinates(deltaTime);

        // Always update the collision/render rect so `player.z` (vertical jump)
        // is applied even when the player is standing still.

        // if (!isEmpty(player.velocity)) update_rect();
        // // here, jsut because first time rect is empty and do it for sakes
        // else if (isEmpty(player.rect)) update_rect();
        set_state();
        update_rect();
        update_animation_speed();
    }

    void debug(SDL_Renderer* renderer) {
        int x = 50;
        int y = 210;
        int lineHeight = 20;
        SDL_SetRenderDrawColor(renderer, 100, 255, 255, 255);

        auto drawLine = [&](const std::string& text) {
            SDL_RenderDebugText(renderer, x, y, text.c_str());
            y += lineHeight;
            };

        // Header
        drawLine("----- PLAYER -----");

        // Position and Grid
        drawLine("(x,y)coord: " + std::to_string(player.x) + ", " + std::to_string(player.y));

        drawLine("(x,y)grid:  " + std::to_string(player.grid.x) + " " + std::to_string(player.grid.y));

        // Map Value
        drawLine("Value: " + std::to_string(map[player.grid.y][player.grid.x]) + " Name: " + std::string(enumMapToString(map[player.grid.y][player.grid.x])));

        // Vectors and Input
        drawLine("(x,y)vel:   " + std::to_string(player.velocity.x) + " " + std::to_string(player.velocity.y));
        drawLine("(x,y)mVec:  " + std::to_string(player.movementVector.x) + " " + std::to_string(player.movementVector.y));
        drawLine("lastKey:    " + std::string(1, player.lastMovementKey)); // Assumes lastMovementKey is a char

        // Speed and State
        drawLine("movementSpeed:  " + std::to_string(player.movementSpeed));

        drawLine("collision: " + std::string(player.collision ? "true" : "false"));

        auto stateVal = stateToString(player.state);
        drawLine("state:    " + std::string(stateVal));
        drawLine("shifting: " + std::to_string(player.shifting) + " animationSpeed: " + std::to_string(player.animationSpeed));

        // player rect
        if (debugText) {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderRect(renderer, &player.rect);
        }
    }
}