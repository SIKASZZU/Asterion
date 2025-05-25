#include "game.h"

double calculate_2_point_distance(double x1, double y1, double x2, double y2);
void update_player_direction(Player& player, SDL_Window* window, struct Offset& offset, int mouse_x, int mouse_y);
void draw_player_direction(SDL_Renderer* renderer, const Player& player, struct Offset& offset, int mouse_x, int mouse_y);