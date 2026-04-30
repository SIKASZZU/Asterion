#pragma once

#include "player.hpp"
#include "map.hpp"

bool check_collision(int map[mapSize][mapSize], struct PlayerData& player, SDL_FRect tempRect);