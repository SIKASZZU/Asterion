#include <SDL2/SDL.h>
#include <algorithm> // for std::find
#include <iostream>

#include "collision.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"

// return boolean 0 or 1
// 1 == collision detected
// 0 == no collision detected  
bool check_collision(int map[map_size][map_size], struct Player& player, SDL_FRect tempRect) {

    // player collision on or off
    if (!player.collision) {
        return false;
    }

    // Calculate grid indices for all four corners
    int grid_x_left = static_cast<int>(tempRect.x / tile_size);
    int grid_x_right = static_cast<int>((tempRect.x + tempRect.w) / tile_size);
    int grid_y_top = static_cast<int>(tempRect.y / tile_size);
    int grid_y_bottom = static_cast<int>((tempRect.y + tempRect.h) / tile_size);

    if (std::find(player.collision_array.begin(), player.collision_array.end(),
        map[grid_y_top][grid_x_left]) != player.collision_array.end()) {
        return true;
    }
    else if (std::find(player.collision_array.begin(), player.collision_array.end(),
        map[grid_y_top][grid_x_right]) != player.collision_array.end()) {
        return true;
    }
    else if (std::find(player.collision_array.begin(), player.collision_array.end(),
        map[grid_y_bottom][grid_x_left]) != player.collision_array.end()) {
        return true;
    }
    else if (std::find(player.collision_array.begin(), player.collision_array.end(),
        map[grid_y_bottom][grid_x_right]) != player.collision_array.end()) {
        return true;
    }
    else {
        return false;
    }
}
