#pragma once

#include "player.hpp"
#include "map.hpp"

bool check_collision(int map[map_size][map_size], struct Player& player, SDL_FRect tempRect);