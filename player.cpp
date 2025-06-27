#include "game.h"
#include <iostream>
#include <cmath>
#include "isometric_calc.h"


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
