#pragma once
#include "player.hpp"
#include <vector>

namespace AnimEnemy {
    const int spriteWidth = 256;
    const int spriteHeight = 256;

    extern Uint32 lastUpdate;  // Used for animation timing
    extern int animCol;
    extern int animRow;
    extern int spriteEnum;
}

namespace AnimPlayer {
    const int spriteWidth = 256;
    const int spriteHeight = 256;
    extern Uint32 lastUpdate;  // Used for animation timing

    extern int lastFrame;
    extern int lastRow;
    extern int currentAnimCol;
    extern int currentAnimRow;
    extern int previousState;
    extern int spriteEnum;

    extern SDL_FPoint lastDirectionVector;
}

void animation_player(SDL_Renderer* renderer);
