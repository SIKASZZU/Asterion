#include "map.hpp"
#include "game.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"
#include "collision.hpp"

#include <cmath>
#include <iostream>

Player player = {
    movement_speed: DEFAULT_PLAYER_MOVEMENT_SPEED,
    size : (tile_size / 2),
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
    SDL_FPoint velocity = { 0.0f, 0.0f };
    bool collision_x = false;
    bool collision_y = false;

    if (dir.x != 0 || dir.y != 0) {
        float normalized_dir = sqrt((dir.x * dir.x) + (dir.y * dir.y));
        velocity.x = dir.x / normalized_dir * player.movement_speed;
        velocity.y = dir.y / normalized_dir * player.movement_speed;

        SDL_FRect playerScreenRect = { player.x  + (tile_size / 2) - (player.size / 2), 
            player.y - (player.size / 4), 
            player.rect.w - (player.size / 2), 
            player.rect.h 
        };
        SDL_FRect rectX = playerScreenRect;
        rectX.x += velocity.x;
        collision_x = check_collision(map, player, rectX);
        
        SDL_FRect rectY = playerScreenRect;
        rectY.y += velocity.y;
        collision_y = check_collision(map, player, rectY);
        
        if (collision_x) { velocity.x = 0; }
        if (collision_y) { velocity.y = 0; }

        player.movement_speed = std::max(std::abs(velocity.x), std::abs(velocity.y));
        player.x += velocity.x;
        player.y += velocity.y;

        SDL_FPoint coords = to_isometric_coordinate(offset, player.x, player.y);
        player.rect = { (coords.x) + (tile_size / 2) - (player.size / 2),
            (coords.y) - (player.size / 4),
            static_cast<float>(player.size),
            static_cast<float>(player.size)
        };
    }
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawRectF(renderer, &player.rect);
}