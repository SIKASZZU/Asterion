#include <SDL2/SDL.h>
#include "collision.h"
#include "isometric_calc.h"
#include "player.h"

#include <iostream>

bool check_collision(int map[map_size][map_size], SDL_FRect tempRect) {
    // Calculate grid indices for all four corners
    int grid_x_left = static_cast<int>(tempRect.x / tile_size);
    int grid_x_right = static_cast<int>((tempRect.x + tempRect.w) / tile_size);
    int grid_y_top = static_cast<int>(tempRect.y / tile_size);
    int grid_y_bottom = static_cast<int>((tempRect.y + tempRect.h) / tile_size);

    // Check if any grid indices are out of bounds
    if (grid_x_left < 0 || grid_x_left >= map_size ||
        grid_x_right < 0 || grid_x_right >= map_size ||
        grid_y_top < 0 || grid_y_top >= map_size ||
        grid_y_bottom < 0 || grid_y_bottom >= map_size) {
        // Handle out of bounds, possibly return true or false based on game logic
        return false;
    }

    // Check all four corners for collision with tile 9
    if (map[grid_x_left][grid_y_top] == 9 ||      // Top-left
        map[grid_x_right][grid_y_top] == 9 ||     // Top-right
        map[grid_x_left][grid_y_bottom] == 9 ||   // Bottom-left
        map[grid_x_right][grid_y_bottom] == 9) {  // Bottom-right
        std::cout << "Encountered alien\n";
        return false;
    }

    return true;
}
