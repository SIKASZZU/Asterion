
#include <SDL2/SDL.h>
#include "game.h"
#include "isometric_calc.h"

// Convert screen coordinate back to grid (tile) coordinate
SDL_FPoint to_grid_coordinate(struct Offset& offset, float screen_x, float screen_y) {
    
    // screen x,y peavad olema grid. Siis teeb ta world gridi -> iso coordiks
    float row_coord = screen_x * (0.5 * tile_size) + screen_y * (-0.5 * tile_size) + offset.x;
    float col_coord = screen_x * (0.25 * tile_size) + screen_y * ( 0.25 * tile_size) + offset.y;
    return { row_coord, col_coord };

}


// Convert grid (tile) coordinate to screen coordinate
SDL_FPoint to_screen_coordinate(struct Offset& offset, float grid_x, float grid_y) {

    float rx = grid_x - offset.x;
    float cy = grid_y - offset.y;
    float x = (rx + 2 * cy) / tile_size;
    float y = (2 * cy - rx) / tile_size;
    
    return {x, y};
}