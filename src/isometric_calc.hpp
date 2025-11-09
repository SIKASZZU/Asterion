#pragma once
#include <SDL3/SDL.h>

#include "game.hpp"
#include "offset.hpp"

SDL_FPoint to_isometric_grid_coordinate(float screenX, float screenY);
SDL_FPoint to_isometric_coordinate(float xGrid, float yGrid);
SDL_FPoint to_screen_coordinate(float xGrid, float yGrid);