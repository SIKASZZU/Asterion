
#include <iostream>
#include <SDL3/SDL.h>
#include "game_state.hpp"

namespace DaylightNS {
    // Day/night system
    extern float timeOfDay; // 0.0 - 1.0
    void toggle_daynight();
    void increase_time_speed();
    void decrease_time_speed();

    void update_daynight(Uint64 elapsedMS);
    // returns brightness 0..1 where 1 = day (bright), 0 = night (dark)
    float get_day_brightness();
    void debug(SDL_Renderer* renderer);
    void draw(GameState* gS);
}

// Game settings struct (runtime-configurable and easy to serialize)
struct Daylight {
    bool day = true;
    bool daylightEnabled = true;
    float dayLengthSeconds = 120.0f;
};
extern Daylight daylightSettings;
