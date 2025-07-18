#include "map.hpp"
#include "game.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"
#include "collision.hpp"

#include <cmath>
#include <iostream>

Player player = {
    movement_speed: DEFAULT_PLAYER_MOVEMENT_SPEED,
    size : tile_size,
    x : 0.0f,
    y : 0.0f,
    rect : {0.0, 0.0, 0.0, 0.0},
    collision : false,
    collision_array : {
        Map::VINE_WALL, Map::WALL_CUBE, Map::INGROWN_WALL_CUBE,
        Map::SECTOR_1_WALL_VAL, Map::SECTOR_2_WALL_VAL, Map::SECTOR_3_WALL_VAL
    },
    movement_vector : {0, 0},
    animation_speed : 125,
    shifting : false,
};


void update_player(int map[map_size][map_size], struct Offset& offset, const Uint8* state, SDL_Renderer* renderer) {
    SDL_FPoint dir = player.movement_vector;  // don't update player.movement_vector var in this func. 

    if (dir.x != 0 || dir.y != 0) {
        // float normalized_dir = sqrt((dir.x * dir.x) + (dir.y * dir.y));
        float speed_x = dir.x * player.movement_speed;
        float speed_y = dir.y * player.movement_speed;
        player.movement_speed = std::max(std::abs(speed_x), std::abs(speed_y));

        // std::cout << "speed: " << speed_x << " " << speed_y << " Saved speed: " << player.movement_speed << "\n";
        
        SDL_FRect tempPlayerRect = { player.x + speed_x, player.y + speed_y, player.rect.w, player.rect.h };
        bool collision = check_collision(map, player, tempPlayerRect);

        if (!collision) {
            player.x += speed_x;
            player.y += speed_y;

            SDL_FPoint coords = to_isometric_coordinate(offset, player.x, player.y);
            player.rect = { (coords.x) + (tile_size / 2) - (player.size / 2),
                            (coords.y) - (player.size / 2),
                            static_cast<float>(player.size),
                            static_cast<float>(player.size)
            };
        }
    }
    SDL_RenderDrawRectF(renderer, &player.rect);
}