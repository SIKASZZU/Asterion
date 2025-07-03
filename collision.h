#pragma once

#include "player.h"
#include "map.h"

bool check_collision(int map[map_size][map_size], struct Player& player, SDL_FRect tempRect);