#include <SDL3/SDL.h>
#include "isometric_calc.hpp"
#include "offset.hpp"
/* camera offset */
Offset offset = { 0, 0 };

void update_offset(struct Offset& offset, struct PlayerData& player) {

    SDL_FPoint coords = to_isometric_coordinate(offset, player.x, player.y);

    coords.x -= offset.x;
    coords.y -= offset.y;

    offset.x = screen_width / 2 - coords.x;
    offset.y = screen_height / 2 - coords.y;
}