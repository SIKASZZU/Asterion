
#include <SDL2/SDL.h>
#include "game.h"
#include <iostream>


SDL_FPoint to_isometric_grid_coordinate(const struct Offset& offset, float x_grid, float y_grid) {
    
    float row_coord = x_grid * (0.5 * tile_size) + y_grid * (-0.5 * tile_size) + offset.x;
    float col_coord = x_grid * (0.25 * tile_size) + y_grid * ( 0.25 * tile_size) + offset.y;

    return { row_coord, col_coord };

}

SDL_FPoint to_isometric_coordinate(const struct Offset& offset, float x_grid, float y_grid) {
    
    float row_coord = x_grid * (0.5) + y_grid * (-0.5) + offset.x;
    float col_coord = x_grid * (0.25) + y_grid * ( 0.25) + offset.y;

    return { row_coord, col_coord };

}


SDL_FPoint to_screen_coordinate(const struct Offset& offset, float coord_x, float coord_y) {

    float rx = coord_x - offset.x;
    float cy = coord_y - offset.y;
    float grid_x = (rx + 2 * cy) / tile_size;
    float grid_y = (2 * cy - rx) / tile_size;
    
    return {grid_x, grid_y};  // grids
}