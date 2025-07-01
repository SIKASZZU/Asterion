#pragma once
#include <SDL2/SDL.h>
#include "game.h"

SDL_FPoint to_grid_coordinate(struct Offset& offset, float screen_x, float screen_y);
SDL_FPoint to_screen_coordinate(struct Offset& offset, float grid_x, float grid_y);