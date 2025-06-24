#include <iostream>
#include <SDL2/SDL.h>
#include "game.h"
#include "isometric_calc.h"

// Invert a 2x2 matrix
struct Matrix2x2 {
    float a, b, c, d;
};


Matrix2x2 invert_matrix(float a, float b, float c, float d) {
    float det = 1.0f / (a * d - b * c);
    return {
        det * d,
        det * -b,
        det * -c,
        det * a
    };
}


// Convert screen coordinate back to grid (tile) coordinate
SDL_Point to_grid_coordinate(int screen_x, int screen_y) {
    float a = i_x * 0.5f;
    float b = j_x * 0.5f;
    float c = i_y * 0.5f;
    float d = j_y * 0.5f;

    Matrix2x2 inv = invert_matrix(a, b, c, d);

    return {
        static_cast<int>(screen_x * inv.a + screen_y * inv.b),
        static_cast<int>(screen_x * inv.c + screen_y * inv.d)
    };
}

// Convert grid (tile) coordinate to screen coordinate
SDL_Point to_screen_coordinate(int grid_x, int grid_y) {
    return {
        static_cast<int>(grid_x * i_x * 0.5f * tile_size + grid_y * j_x * 0.5f * tile_size),
        static_cast<int>(grid_x * i_y * 0.5f * tile_size + grid_y * j_y * 0.5f * tile_size)
    };
}