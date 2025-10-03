#include "player.hpp"

namespace AnimEnemy {
    extern Uint32 lastUpdate;  // Used for animation timing
    extern int lastFrame;      // Remember index of last anim
    extern int animCol;
    extern int animRow;
}

namespace AnimPlayer {
    extern Uint32 lastUpdate;  // Used for animation timing
    extern int lastFrame;      // Remember index of last player anim
    extern int lastRow;        // Used for accessing player anim texture
}

void animation_player(SDL_Renderer* renderer);
