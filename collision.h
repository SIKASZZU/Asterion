
#include "player.h"
#include "map.h"

void check_collision(int map[map_size][map_size], struct Player& player, struct Offset& offset, SDL_Rect destTile);
void update_collision(int map[map_size][map_size], struct Player& player, struct Offset& offset, SDL_Rect destTile);