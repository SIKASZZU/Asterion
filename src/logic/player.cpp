#include <cmath>
#include <iostream>

#include "map.hpp"
#include "game.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"
#include "collision.hpp"
#include "offset.hpp"

const SDL_Point spawnpointGrid = { x: mapSize / 2, y: mapSize / 2 };

PlayerData player = {
    movementSpeed: PlayerNS::defaultMovementSpeed,
    size : (tileSize),
    gridX : spawnpointGrid.x,
    gridY : spawnpointGrid.y,
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
    float tilesPerSecond = 25.0f;
    float defaultMovementSpeed = tileSize * tilesPerSecond;

    void create_movementVector(const bool* state) {
        SDL_FPoint dir = player.movementVector;
        if (state[SDL_SCANCODE_W] && !PlayerNS::collisionY) {
            dir.y = -1;
            player.lastMovementKey = 'w';
            if (player.cartesianMovement == true) {
                dir.x = -1;
            };
        }
        if (state[SDL_SCANCODE_S] && !PlayerNS::collisionY) {
            dir.y = 1;
            player.lastMovementKey = 's';
            if (player.cartesianMovement == true) {
                dir.x = 1;
            };
        }
        if (state[SDL_SCANCODE_A] && !PlayerNS::collisionX) {
            dir.x = -1;
            player.lastMovementKey = 'a';
            if (player.cartesianMovement == true) {
                dir.y = 1;
            };
        }
        if (state[SDL_SCANCODE_D] && !PlayerNS::collisionX) {
            dir.x = 1;
            player.lastMovementKey = 'd';
            if (player.cartesianMovement == true) {
                dir.y = -1;
            };
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

    void update(int map[mapSize][mapSize], struct ::Offset& offset, SDL_Renderer* renderer, float deltaTime) {
        SDL_FPoint dir = player.movementVector;
        SDL_FPoint velocity = { 0.0f, 0.0f };
        collisionX = false;
        collisionY = false;
        if (dir.x != 0 || dir.y != 0) {
            // if player presses movement key then reset the sliding with giga hard speed
            if (abs(dir.x) == 1 || abs(dir.y) == 1) {
                if (!player.shifting) { player.movementSpeed = defaultMovementSpeed; }
                else { player.movementSpeed = defaultMovementSpeed / 4; }
            }
            velocity.x = dir.x * player.movementSpeed * deltaTime;
            velocity.y = dir.y * player.movementSpeed * deltaTime;
            SDL_FRect templatePlayerRect = {
                player.x,
                player.y,
                player.rect.w,
                player.rect.h
            };
            SDL_FRect rectX = templatePlayerRect;
            rectX.x += velocity.x;
            collisionX = check_collision(map, player, rectX);

            SDL_FRect rectY = templatePlayerRect;
            rectY.y += velocity.y;
            collisionY = check_collision(map, player, rectY);

            if (collisionX) {
                velocity.x = 0;
            }
            if (collisionY) {
                velocity.y = 0;
            }
            player.x += velocity.x;
            player.y += velocity.y;
            // update logical grid coordinates after motion
            player.gridX = static_cast<int>((player.x + player.size / 2.0f) / tileSize);
            player.gridY = static_cast<int>((player.y + player.size / 2.0f) / tileSize);
        }
        else {
            // muidu player seisab aga variable ytleb, et speed == 20
            player.movementSpeed = 0;
        }
        SDL_FPoint coords = to_isometric_coordinate(player.x, player.y);
        player.rect = { coords.x + player.size / 4.0f, // + 
            coords.y - player.size / 4.0f, //  
            player.size,
            player.size
        };
    }
}