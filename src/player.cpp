#include <cmath>
#include <iostream>

#include "map.hpp"
#include "game.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"
#include "collision.hpp"
#include "offset.hpp"

PlayerData player = {
    movement_speed: PlayerNS::DEFAULT_MOVEMENT_SPEED,
    size : (tile_size / 2),
    x : map_size / 2 * tile_size,
    y : map_size / 2 * tile_size,
    rect : {0.0, 0.0, 0.0, 0.0},
    collision : false,
    collision_array : wall_values,
    movement_vector : {0, 0},
    velocity : {0, 0},
    animation_speed : 1,
    shifting : false,
    last_movement_key : 'a',
};

namespace PlayerNS {
    bool collisionX = false;
    bool collisionY = false;
    float tilesPerSecond = 5.0f;
    float DEFAULT_MOVEMENT_SPEED = tile_size * tilesPerSecond;

    void update(int map[map_size][map_size], struct ::Offset& offset, SDL_Renderer* renderer, float deltaTime) {
        // don't update player.movement_vector var in this func.
        SDL_FPoint dir = player.movement_vector;
        SDL_FPoint velocity = { 0.0f, 0.0f };
        collisionX = false;
        collisionY = false;
        if (dir.x != 0 || dir.y != 0) {
            // if player presses movement key then reset the sliding with giga hard speed
            if (abs(dir.x) == 1 || abs(dir.y) == 1) {
                if (!player.shifting) { player.movement_speed = DEFAULT_MOVEMENT_SPEED; }
                else { player.movement_speed = DEFAULT_MOVEMENT_SPEED / 4; }
            }
            velocity.x = dir.x * player.movement_speed * deltaTime;
            velocity.y = dir.y * player.movement_speed * deltaTime;
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
        }
        else {
            // muidu player seisab aga variable ytleb, et speed == 20
            player.movement_speed = 0;
        }
        SDL_FPoint coords = to_isometric_coordinate(offset, player.x, player.y);
        player.rect = { coords.x + player.size / 2,
            coords.y - player.size / 2,
            player.size,
            player.size
        };
    }
}