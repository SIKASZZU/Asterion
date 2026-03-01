#include <cmath>
#include <iostream>

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
    lastMovementKey : 'a',
    cartesianMovement : false,
};



namespace PlayerNS {
    bool collisionX = false;
    bool collisionY = false;
    float tilesPerSecond = 10.0f;
    float defaultMovementSpeed = tileSize * tilesPerSecond;
    float defaultMovementSpeedShifting = tileSize * tilesPerSecond / 4;

    void create_movementVector(const bool* state) {
        SDL_FPoint dir = { 0, 0 };
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

    SDL_FPoint validate_velocity(SDL_FPoint& newVelocity) {
        SDL_FRect templatePlayerRect = {
            player.x,
            player.y,
            // see / 2 on h2kk, sest player size on nyyd full tileSize. COllision arvutamise m6ttes, tegin /2
            player.rect.w / 2,
            player.rect.h / 2
        };

        SDL_FRect rectX = templatePlayerRect;
        rectX.x += newVelocity.x;
        collisionX = check_collision(map, player, rectX);

        SDL_FRect rectY = templatePlayerRect;
        rectY.y += newVelocity.y;
        collisionY = check_collision(map, player, rectY);

        if (collisionX) {
            newVelocity.x = 0;
        }
        if (collisionY) {
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
            coords.y - (player.size / 4) + offset.y - groundOffsetAmount,
            player.size,
            player.size
        };
    }

    void calculate_new_coordinates(float deltaTime) {
        SDL_FPoint dir = player.movementVector;
        SDL_FPoint velocity = { 0.0f, 0.0f };

        collisionX = false;
        collisionY = false;

        if (dir.x != 0 || dir.y != 0) {
            // if player presses movement key then reset the sliding with giga hard speed
            if (abs(dir.x) == 1 || abs(dir.y) == 1) {
                if (!player.shifting) { player.movementSpeed = defaultMovementSpeed; }
                else { player.movementSpeed = defaultMovementSpeedShifting; }
            }
            velocity.x = dir.x * player.movementSpeed * deltaTime;
            velocity.y = dir.y * player.movementSpeed * deltaTime;

            velocity = validate_velocity(velocity);

            player.x += velocity.x;
            player.y += velocity.y;
            player.grid = {
                static_cast<int>((player.x + player.size / 2.0f) / tileSize),
                static_cast<int>((player.y + player.size / 2.0f) / tileSize)
            };
        }
        else {
            // muidu player seisab aga variable ytleb, et speed == 20
            player.movementSpeed = 0;
        }
    }

    void setState() {
        if (player.movementSpeed == 0) {
            player.state = PlayerState::Idle;
            return;
        }
        if (player.shifting) {
            player.state = PlayerState::Walk;
            return;
        }
        else if (!player.shifting){
            player.state = PlayerState::Run;
            return;
        }
        player.state = PlayerState::Idle;
        return;
    }

    void update(int map[mapSize][mapSize], struct ::Offset& offset, SDL_Renderer* renderer, float deltaTime) {
        calculate_new_coordinates(deltaTime);
        if (player.movementSpeed != 0) update_rect();
        setState();

        // Apply ground offset only for rendering

    }
}