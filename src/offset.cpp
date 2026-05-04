#include <SDL3/SDL.h>
#include "isometric_calc.hpp"
#include "offset.hpp"
#include <iostream>
#include "player.hpp"

/* camera offset */
Offset offset = { 0, 0, false };

void update_offset(struct PlayerData& player) {

    SDL_FPoint coords = to_isometric_coordinate(player.x, player.y);

    offset.x = screenWidth / 2 - coords.x;
    offset.y = screenHeight / 2 - coords.y;
}