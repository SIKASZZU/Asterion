#include "game.h"
#include <iostream>


void update_player_direction(Player& player, SDL_Window* window, struct Offset& offset, int mouse_x, int mouse_y) {

    int world_mouse_x = mouse_x + offset.x;
    int world_mouse_y = mouse_y + offset.y;

    double dx = world_mouse_x - player.x;
    double dy = world_mouse_y - player.y;

    player.direction = std::atan2(dy, dx);  // radians, counter-clockwise
    // std::cout << "player direction: " << player.direction << '\n';
}

/* see sitt on overkill ning vajab nahhui viskamist (kiiremas korras) */
double calculate_2_point_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void draw_player_direction(SDL_Renderer* renderer, const Player& player, struct Offset& offset, int mouse_x, int mouse_y) {
    int x1 = player.x;
    int y1 = player.y;
    int world_mouse_x = mouse_x + offset.x;
    int world_mouse_y = mouse_y + offset.y;
    double distance = calculate_2_point_distance(x1, y1, world_mouse_x, world_mouse_y);  // et vaid cursorini drawiks vektori

    int x2 = x1 + std::cos(player.direction) * distance;
    int y2 = y1 + std::sin(player.direction) * distance;

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
    SDL_RenderDrawLine(renderer, x1 - offset.x, y1 - offset.y, 
                                x2 - offset.x, y2 - offset.y);
}
