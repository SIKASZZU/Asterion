#include <SDL3/SDL.h>
#include <algorithm> // for std::find
#include <iostream>

#include "collision.hpp"
#include "isometric_calc.hpp"
#include "player.hpp"
#include "render.hpp"

// return boolean 0 or 1
// 1 == collision detected
// 0 == no collision detected  
bool check_collision(int map[mapSize][mapSize], struct PlayerData& player, SDL_FRect tempRect) {

    // player collision on or off
    if (!player.collision) {
        return false;
    }

    // Calculate grid indices for all four corners
    int grid_x_left = static_cast<int>(tempRect.x / tileSize);
    int grid_x_right = static_cast<int>((tempRect.x + tempRect.w) / tileSize);
    int grid_y_top = static_cast<int>(tempRect.y / tileSize);
    int grid_y_bottom = static_cast<int>((tempRect.y + tempRect.h) / tileSize);

    // For each corner: if the grid value is a collidable value and
    // the grid is NOT in sector3Cutouts, count it as a collision.
    uint32_t key_tl = make_grid_key(grid_y_top, grid_x_left);
    int val_tl = map[grid_y_top][grid_x_left];
    if (player.collision_array.find(val_tl) != player.collision_array.end()
        && sector3Cutouts.find(key_tl) == sector3Cutouts.end()) {
        return true;
    }

    uint32_t key_tr = make_grid_key(grid_y_top, grid_x_right);
    int val_tr = map[grid_y_top][grid_x_right];
    if (player.collision_array.find(val_tr) != player.collision_array.end()
        && sector3Cutouts.find(key_tr) == sector3Cutouts.end()) {
        return true;
    }

    uint32_t key_bl = make_grid_key(grid_y_bottom, grid_x_left);
    int val_bl = map[grid_y_bottom][grid_x_left];
    if (player.collision_array.find(val_bl) != player.collision_array.end()
        && sector3Cutouts.find(key_bl) == sector3Cutouts.end()) {
        return true;
    }

    uint32_t key_br = make_grid_key(grid_y_bottom, grid_x_right);
    int val_br = map[grid_y_bottom][grid_x_right];
    if (player.collision_array.find(val_br) != player.collision_array.end() 
        && sector3Cutouts.find(key_br) == sector3Cutouts.end()) {
        return true;
    }
    else {
        return false;
    }
}
