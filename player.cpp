#include "map.hpp"
#include "game.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"
#include "collision.hpp"

#include <cmath>
#include <iostream>

Player player = {
    movement_speed : DEFAULT_PLAYER_MOVEMENT_SPEED,
    size : 64,
    x : 0.0f,
    y : 0.0f,
    rect : {0.0, 0.0, 0.0, 0.0},
    collision : false,
    collision_array : {Map::WALL_VAL_VINE, Map::WALL_VAL,
                    Map::SECTOR_1_WALL_VAL, Map::SECTOR_2_WALL_VAL,
                    Map::SECTOR_3_WALL_VAL, Map::INGROWN_WALL_VAL
                    },
    movement_vector : {0, 0},
};


void update_player(int map[map_size][map_size], struct Offset& offset, const Uint8* state, SDL_Renderer* renderer) {
    SDL_FPoint dir = player.movement_vector;

    if (dir.x != 0 || dir.y != 0) {
        float normalized_dir = sqrt((dir.x * dir.x) + (dir.y * dir.y));  // normalize diagonal movement

        dir.x = dir.x / normalized_dir * player.movement_speed;
        dir.y = dir.y / normalized_dir * player.movement_speed;

        SDL_FRect tempPlayerRect = { player.x + dir.x, player.y + dir.y, player.rect.w, player.rect.h };
        bool collision = check_collision(map, player, tempPlayerRect);

        if (!collision) {
            player.x += dir.x;
            player.y += dir.y;

            SDL_FPoint coords = to_isometric_coordinate(offset, player.x, player.y);
            player.rect = { (coords.x) + (tile_size / 2) - (player.size / 2),
                    (coords.y) - (player.size / 2),
                    static_cast<float>(player.size),
                    static_cast<float>(player.size)
            };
        }
    }
}