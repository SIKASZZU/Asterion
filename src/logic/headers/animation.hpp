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
    extern int lastFrame;      // Remember index of last player anim
    extern int lastRow;        // Used for accessing player anim texture
    extern int animCol;
    extern SDL_FPoint lastDirection;
    extern int spriteEnum;
    extern SDL_FRect lastSrcRect;
}

void animation_player(SDL_Renderer* renderer);
