#include "game.h"

void update_player_direction(Player& player, SDL_Window* window, struct Offset& offset);
void draw_player_direction(SDL_Renderer* renderer, const Player& player, struct Offset& offset);