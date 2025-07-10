#pragma once // compilerile relvaga kulpi, et jooksutaks seda faili vaid yhe korra
#include <SDL2/SDL.h>
#include <functional>

#include "player.h"
#include "map.h"

// /* screen */
extern int win_width;
extern int win_height;
extern int mouse_x;
extern int mouse_y;

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

/* render.h args */
extern int render_radius;

/* map.h args */
extern int tile_size; 

/* pathfinding */
extern int pathEndX;
extern int pathEndY;
extern int pathStartX;
extern int pathStartY;

/* keys */
extern bool shift_pressed;

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second << 1);
    }
};

/* functions */
void update_offset(struct Offset& offset, struct Player& player, SDL_Window* window);
void call_set_functionality(SDL_Keycode key_pressed, struct Player& player, struct Offset& offset, int map[map_size][map_size]);