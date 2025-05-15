#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "game.h"


int map[map_size][map_size];  // tra mdea, aga see peab siin uuesti olema ilma externita.

Player player = {
    5, 32, 0, 0
};

bool isRunning = true;

/* framerate */
int frame_count = 0;
float fps = 0.0f;
Uint32 fps_timer;
Uint32 frame_start;
Uint32 frame_time;

/* tickrate */
Uint32 previous_tick = SDL_GetTicks();
Uint32 tick_lag = 0;
const int tickrate = 60;
const int tick_delay = 1000 / tickrate;
int tick_count = 0;
float ticks_per_second = 0.0f;
Uint32 tick_timer = SDL_GetTicks();

void render_map(SDL_Renderer* renderer, const int tile_size, struct Offset& offset) {
    /* Render map array info */
    /* 1 = Ground, background = blue */

    for (int column = 0; column < map_size; column++) {
        for (int row = 0; row < map_size; row++) {
            SDL_Rect tile = {
                    column * tile_size + offset.x,
                    row * tile_size + offset.y,
                    tile_size,
                    tile_size
                };

            switch (map[column][row]) {
            
            case 1:
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &tile);
                continue;
                
            case 2:
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderFillRect(renderer, &tile);
                continue;
            
            default:
                continue;
            }
        }
    }
}


int random_number_gen(int size){
    /* size arg = (from 1 until size) */
    if (size < 1) { size = 1; };

    srand(time(NULL));
    int number = rand() % size + 1;

    std::cout << number << '\n';

    return number;
}


void generate_random_map(int map[map_size][map_size], int min_val, int max_val) {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    float center_x = map_size / 2.0f;
    float center_y = map_size / 2.0f;
    float max_distance = std::sqrt(center_x * center_x + center_y * center_y);

    for (int x = 0; x < map_size; x++) {
        for (int y = 0; y < map_size; y++) {
            float dx = x - center_x;
            float dy = y - center_y;
            float distance = std::sqrt(dx * dx + dy * dy) / max_distance;

            // Inverse chance of land (closer to center = more land)
            float land_chance = 1.0f - distance;

            // Add stronger noise (-0.4 to +0.4)
            land_chance += ((std::rand() % 100) / 100.0f - 0.5f) * 0.8f;

            // Clamp to 0–1
            if (land_chance < 0.0f) land_chance = 0.0f;
            if (land_chance > 1.0f) land_chance = 1.0f;

            if (land_chance > 0.4f) {
                // Land: elevation between mid and max
                int land_val = min_val + (max_val - min_val) / 2 + std::rand() % ((max_val - min_val) / 2 + 1);
                map[x][y] = land_val;
            } else {
                // Water
                map[x][y] = min_val;
            }
        }
    }
}


void print_map(int map[map_size][map_size]) {
    for (int x = 0; x < map_size; x++) {
        for (int y = 0; y < map_size; y++) {
            std::cout << map[x][y] << ' ';
        }
        std::cout << '\n';
    }std::cout << std::endl;
}


void update_offset(struct Offset& offset) {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_W]) {
        offset.y += player.movement_speed;
    }
    
    if (state[SDL_SCANCODE_S]) {
        offset.y -= player.movement_speed;
    }
    
    if (state[SDL_SCANCODE_A]) {
        offset.x += player.movement_speed;
    }
    
    if (state[SDL_SCANCODE_D]) {
        offset.x -= player.movement_speed;
    }
}

void update_player(struct Offset& offset){

    // kui offset x on neg, siis muuta pos ja kui x on pos siis muuta negiks.
    player.x = -offset.x;
    player.y = -offset.y;

    // std::cout << "player x,y " << player.x << ' ' << player.y << '\n';
    // std::cout << "offset x,y " << offset.x << ' ' << offset.y << '\n';
    // std::cout << std::endl;
}