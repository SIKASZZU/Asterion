#include "game.h"
#include "isometric_calc.h"
#include "player.h"
#include <cmath>
#include "collision.h"

Player player = {
    20.0f,                // float      default movement_speed, seda ei muuda ns
    20.0f,                // float      movement_speed
    32,                   // int        size
    0.0f,                 // float      x
    0.0f,                 // float      y
    0.0,                  // double     direction
    {0.0, 0.0, 0.0, 0.0}, // SDL_FRect  rect
    false,                // bool       collision
    false                 // bool       render
};


void update_player(int map[map_size][map_size], struct Offset& offset, const Uint8* state, SDL_Renderer* renderer) {
    SDL_FPoint dir = {0,0};

    if (state[SDL_SCANCODE_W]) { dir.y -= 1; }
    if (state[SDL_SCANCODE_S]) { dir.y += 1; }
    if (state[SDL_SCANCODE_A]) { dir.x -= 1; }
    if (state[SDL_SCANCODE_D]) { dir.x += 1; }

    
    if (dir.x != 0 || dir.y != 0) {
        float normalized_dir = sqrt((dir.x * dir.x) + (dir.y * dir.y));  // normalize diagonal movement
        
        dir.x = dir.x / normalized_dir * player.movement_speed;
        dir.y = dir.y / normalized_dir * player.movement_speed;

        SDL_FRect tempPlayerRect = { player.x + dir.x, player.y + dir.y, player.rect.w, player.rect.h};
        bool accessible = check_collision(map, player, tempPlayerRect);
        
        if (accessible) { 
            player.x += dir.x;
            player.y += dir.y;
        }
    }
}


void draw_player(SDL_Renderer* renderer, const struct Offset& offset) {

    // convert to isometric
    float row_coord = player.x * (0.5) + player.y * (-0.5) + offset.x;
    float col_coord = player.x * (0.25) + player.y * (0.25) + offset.y;

    //   + (tile_size / 2) - (player.size / 2)
    player.rect = {row_coord + (tile_size / 2) - (player.size / 2), col_coord - (player.size / 2), static_cast<float>(player.size), static_cast<float>(player.size)};
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); //green
    SDL_RenderFillRectF(renderer, &player.rect);
}


void update_player_direction(struct Player& player, SDL_Window* window, struct Offset& offset, int mouse_x, int mouse_y) {
    // Convert player's world position to isometric screen position
    SDL_FPoint iso_player = to_grid_coordinate(offset, player.x, player.y);

    double dx = mouse_x - (iso_player.x - offset.x);
    double dy = mouse_y - (iso_player.y - offset.y);

    player.direction = std::atan2(dy, dx);  // Radians
}


/* see sitt on overkill ning vajab nahhui viskamist (kiiremas korras) */
double calculate_2_point_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}


void draw_player_direction(SDL_Renderer* renderer, struct Player& player, struct Offset& offset, int mouse_x, int mouse_y) {
    int x1 = player.x;
    int y1 = player.y;

    int world_mouse_x = mouse_x + offset.x;
    int world_mouse_y = mouse_y + offset.y;

    double distance = calculate_2_point_distance(x1, y1, world_mouse_x, world_mouse_y);

    int x2 = x1 + std::cos(player.direction) * distance;
    int y2 = y1 + std::sin(player.direction) * distance;

    // Convert both points to isometric screen coordinates
    SDL_FPoint iso_start = to_grid_coordinate(offset, x1, y1);
    SDL_FPoint iso_end   = to_grid_coordinate(offset, x2, y2);

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
    SDL_RenderDrawLine(renderer, iso_start.x, iso_start.y, iso_end.x, iso_end.y);
}
