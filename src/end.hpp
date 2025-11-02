#include <SDL3/SDL.h>

namespace Portal {

    bool has_entered();
}

namespace Ending {
    extern bool start;
    void update(SDL_Renderer* renderer); 
}