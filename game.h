#pragma once // compilerile relvaga kulpi, et jooksutaks seda faili vaid yhe korra
#include <SDL2/SDL.h>
#include "map.h"

// /* screen */
extern int win_width;
extern int win_height;
extern int mouse_x;
extern int mouse_y;

/* camera offset */
struct Offset {
    int x;
    int y;
};

/* movement_speed, size, x, y, direction (facing) */
struct Player {
    int movement_speed;
    int size;
    int x;
    int y;
    double direction;
};

extern Player player;

/* game */
extern bool isRunning;
extern int render_radius;
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

/* functions */
int random_number_gen(int min, int max);
void update_offset(struct Offset& offset, int win_width, int win_height);
void update_player(struct Offset& offset, const Uint8* state);
void call_set_functionality(SDL_Keycode key_pressed, struct Offset& offset);