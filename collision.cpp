#include <SDL2/SDL.h>
#include "collision.h"
#include "isometric_calc.h"
#include "player.h"


void check_collision(int map[map_size][map_size], struct Player& player, 
                    struct Offset& offset, SDL_Rect destTile) {

    if (map[static_cast<int>(player.y / tile_size)][static_cast<int>(player.x / tile_size)] != 9) {
        return;
    }
    
    SDL_FPoint iso_wall = to_screen_coordinate(offset, destTile.y, destTile.x);
    SDL_Rect wall_rect = {static_cast<int>(iso_wall.x), static_cast<int>(iso_wall.y), 
                        tile_size, tile_size};
    
    SDL_FPoint iso_p = to_screen_coordinate(offset, player.rect.x, player.rect.y);
    SDL_Rect p_rect = {static_cast<int>(iso_p.x), static_cast<int>(iso_p.y), 
                    static_cast<int>(player.rect.w), static_cast<int>(player.rect.h)};

    // alati true aga lihtsalt fuck you
    SDL_Rect overlap;
    if (!SDL_IntersectRect(&p_rect, &wall_rect, &overlap)) {
        
    } 

    if (overlap.w < overlap.h) {
        // Horizontal collision: resolve on X axis
        if (p_rect.x < wall_rect.x) {
            player.x -= overlap.w;  // push left
        } else {
            player.x += overlap.w;  // push right
        }
        // player.velX = 0;

    } else {
        // Vertical collision: resolve on Y axis
        if (p_rect.y < wall_rect.y) {
            player.y -= overlap.h;  // push up
        } else {
            player.y += overlap.h;  // push down
        }
        // player.velY = 0;

    }

}


void update_collision(int map[map_size][map_size], struct Player& player, 
                    struct Offset& offset, SDL_Rect destTile) {
    
    check_collision(map, player, offset, destTile);
}
