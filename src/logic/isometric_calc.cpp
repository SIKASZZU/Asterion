
#include <SDL3/SDL.h>
#include <iostream>
#include "game.hpp"
#include "offset.hpp"

/// @brief idfk, Does multiply with tileSize.
/// @param xGrid X-grid
/// @param yGrid Y-grid
/// @return Returns coordinates.
SDL_FPoint to_isometric_grid_coordinate(float xGrid, float yGrid) {
    float rowCoord = xGrid * (0.5 * tileSize) + yGrid * (-0.5 * tileSize) + offset.x;
    float colCoord = xGrid * (0.25 * tileSize) + yGrid * (0.25 * tileSize) + offset.y;
    return { rowCoord, colCoord };

}
/// @brief idfk, doesn't multiply with tileSize.
/// @param xGrid X-grid
/// @param yGrid Y-grid
/// @return Returns coordinates.
SDL_FPoint to_isometric_coordinate(float xGrid, float yGrid) {
    float rowCoord = xGrid * (0.5) + yGrid * (-0.5);
    float colCoord = xGrid * (0.25) + yGrid * (0.25);
    return { rowCoord, colCoord };

}
/// @brief 
/// @param rowCoord X-coordinate
/// @param colCoord Y-coordinate
/// @return Returns grid coords. return { xGrid, yGrid };
SDL_FPoint to_screen_coordinate(float rowCoord, float colCoord) {
    float rx = rowCoord - offset.x;
    float cy = colCoord - offset.y;
    float xGrid = (rx + 2 * cy) / tileSize;
    float yGrid = (2 * cy - rx) / tileSize;
    return { xGrid, yGrid };
}