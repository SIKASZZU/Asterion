#pragma once
#include "game_state.hpp"
#include "player.hpp"

/* camera offset */
struct Offset {
    float x;
    float y;
    bool flipped = false;
};
extern Offset offset;

void update_offset(GameState* gS, struct PlayerData& player);