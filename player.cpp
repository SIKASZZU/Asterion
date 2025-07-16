#include <cmath>
#include <iostream>

#include "game.h"
#include "isometric_calc.h"
#include "player.h"
#include "collision.h"

Player player = {
    DEFAULT_PLAYER_MOVEMENT_SPEED,      // float                movement_speed
    64,                                 // int                  size
    0.0f,                               // float                x
    0.0f,                               // float                y
    {0.0, 0.0, 0.0, 0.0},               // SDL_FRect            rect
    false,                              // bool                 collision
    {8, 9, 91, 92, 93, 94},             // vector               collision_array
    {0, 0},                             // SDL_FPoint           movement_vector
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