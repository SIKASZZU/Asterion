#pragma once
#include <SDL3/SDL.h>

#include "game.hpp"
#include "offset.hpp"

SDL_FPoint to_isometric_grid_coordinate(const struct Offset& offset, float screen_x, float screen_y);
SDL_FPoint to_isometric_coordinate(const struct Offset& offset, float x_grid, float y_grid);
SDL_FPoint to_screen_coordinate(const struct Offset& offset, float grid_x, float grid_y);