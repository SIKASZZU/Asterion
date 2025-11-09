#pragma once
#include <SDL3/SDL.h>

#include "game.hpp"
#include "offset.hpp"

SDL_FPoint to_isometric_grid_coordinate(float screen_x, float screen_y);
SDL_FPoint to_isometric_coordinate(float x_grid, float y_grid);
SDL_FPoint to_screen_coordinate(float grid_x, float grid_y);