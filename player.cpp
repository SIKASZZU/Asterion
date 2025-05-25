#include "game.h"
#include <iostream>


void update_player_direction(Player& player, SDL_Window* window, struct Offset& offset) {
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);  // a 32-bit button bitmask of the current button state. Mis tra asi see on?

    int world_mouse_x = mouse_x + offset.x;
    int world_mouse_y = mouse_y + offset.y;

    double dx = world_mouse_x - player.x;
    double dy = world_mouse_y - player.y;

    player.direction = std::atan2(dy, dx);  // radians, counter-clockwise
    std::cout << "player direction: " << player.direction << '\n';
}


void draw_player_direction(SDL_Renderer* renderer, const Player& player, struct Offset& offset) {
    int x1 = player.x;
    int y1 = player.y;

    int x2 = x1 + std::cos(player.direction) * 100;
    int y2 = y1 + std::sin(player.direction) * 100;

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
    SDL_RenderDrawLine(renderer, x1 - offset.x, y1 - offset.y, 
                                x2 - offset.x, y2 - offset.y);
}
