
#include <SDL3/SDL.h>
#include <iostream>
#include "game.hpp"
#include "offset.hpp"

/// @brief idfk, Does multiply with tileSize.
/// @param xGrid X-grid
/// @param yGrid Y-grid
/// @return Returns coordinates.
SDL_FPoint to_isometric_grid_coordinate(float xGrid, float yGrid) {
    // base transform (world grid -> isometric screen coords)
    float baseRow = xGrid * (0.5f * tileSize) + yGrid * (-0.5f * tileSize);
    float baseCol = xGrid * (0.25f * tileSize) + yGrid * (0.25f * tileSize);

    float rowCoord = offset.flipped ? -baseRow + offset.x : baseRow + offset.x;
    float colCoord = offset.flipped ? -baseCol + offset.y : baseCol + offset.y;
    return { rowCoord, colCoord };

}
/// @brief idfk, doesn't multiply with tileSize.
/// @param xGrid X-grid
/// @param yGrid Y-grid
/// @return Returns coordinates.
SDL_FPoint to_isometric_coordinate(float xGrid, float yGrid) {
    float baseRow = xGrid * (0.5f) + yGrid * (-0.5f);
    float baseCol = xGrid * (0.25f) + yGrid * (0.25f);
    float rowCoord = offset.flipped ? -baseRow : baseRow;
    float colCoord = offset.flipped ? -baseCol : baseCol;
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
    if (offset.flipped) {
        xGrid = -xGrid;
        yGrid = -yGrid;
    }
    return { xGrid, yGrid };
}