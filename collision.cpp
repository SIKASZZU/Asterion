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

    // Check all four corners for collision with tile 9
    if (map[grid_y_top][grid_x_left] == 9 ||      // Top-left
        map[grid_y_top][grid_x_right] == 9 ||     // Top-right
        map[grid_y_bottom][grid_x_left] == 9 ||   // Bottom-left
        map[grid_y_bottom][grid_x_right] == 9) {  // Bottom-right
        std::cout << "Encountered alien\n";
        return false;
    
    } else{ 

        return true;

    }

}
