#include <iostream>
#include <SDL2/SDL.h>

SDL_Point world_to_iso(int x, int y) {
    /* Only coordinates, not tiles! */
    SDL_Point iso;
    iso.x = (x - y) / 2;
    iso.y = (x + y) / 4;
    return iso;
}


SDL_Point iso_to_world(int iso_x, int iso_y) {
    /* Only coordinates, not tiles! */
    SDL_Point world;
    world.x = iso_x + 2 * iso_y;
    world.y = 2 * iso_y - iso_x;
    return world;
}