#pragma once // compilerile relvaga kulpi, et jooksutaks seda faili vaid yhe korra
#include <SDL3/SDL.h>
#include <functional>

#include "player.hpp"
#include "map.hpp"
#include "enemy.hpp"

// /* screen */
extern int screen_width;
extern int screen_height;
extern float mouse_x;
extern float mouse_y;

/* camera offset */
struct Offset {
    float x;
    float y;
};

/* game */
extern bool isRunning;
extern const Uint8* state;

/* framerate */
extern Uint32 frame_start;
extern Uint32 frame_time;
extern int frame_count;
extern float fps;
extern Uint32 fps_timer;

/* tickrate */
extern const int tickrate;
extern const int tick_delay;
extern Uint32 previous_tick;
extern Uint32 tick_lag;
extern int tick_count;
extern float ticks_per_second;
extern Uint32 tick_timer;

/* render.hpp args */
extern int render_radius;

/* map.hpp args */
extern float tile_size;

/* pathfinding */
extern int pathEndX;
extern int pathEndY;
extern int pathStartX;
extern int pathStartY;

/* keys */
extern bool shift_pressed;
extern bool r_pressed;
extern bool v_pressed;

/* constants */
const double PI = 3.1415926535897932384626433832;

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second << 1);
    }
};

/* functions */
void update_offset(struct Offset& offset, struct Player& player);
void react_to_keyboard_down(SDL_Keycode key, struct Player& player, struct Offset& offset, int map[map_size][map_size]);
void react_to_keyboard_up(SDL_Keycode key, struct Player& player);
void react_to_keyboard_state(const bool* state, struct Player& player);
