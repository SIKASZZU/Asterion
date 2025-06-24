#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "game.h"


constexpr float i_x = 1.0f;
constexpr float i_y = 0.5f;
constexpr float j_x = -1.0f;
constexpr float j_y = 0.5f;

struct Matrix2x2;
SDL_Point to_grid_coordinate(int screen_x, int screen_y);
SDL_Point to_screen_coordinate(int grid_x, int grid_y);
Matrix2x2 invert_matrix(float a, float b, float c, float d);