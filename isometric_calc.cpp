#include <iostream>
#include <SDL2/SDL.h>

SDL_Point world_to_iso(int x, int y) {
    SDL_Point iso;
    iso.x = (x - y) / 2;
    iso.y = (x + y) / 4;
    return iso;
}