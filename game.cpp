#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "game.h"


int map[map_size][map_size];  // tra mdea, aga see peab siin uuesti olema ilma externita.

Player player = {
    9, 32, 0, 0
};

bool isRunning = true;
int win_width;
int win_height;
int mouse_x = 0;
int mouse_y = 0;

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
int render_radius = 30;  // perfectse rad -> (win_width / 2) / tile_size //*NOTE win_widthil pole siin veel v22rtust vaid

void render_map(SDL_Renderer* renderer, const int tile_size, struct Offset& offset,
                SDL_Texture* ground_tex, SDL_Texture* tree_tex) {
    int player_tile_y = player.y / tile_size;
    int bottom = player_tile_y + render_radius;
    int top    = player_tile_y - render_radius;

    int player_tile_x = player.x / tile_size;
    int left   = player_tile_x - render_radius;
    int right  = player_tile_x + render_radius;

    // Pass 1: Render all ground tiles
    for (int row = 0; row < map_size; row++) {
        if (row < top || row > bottom) continue;
        
        for (int column = 0; column < map_size; column++) {
            if (column < left || column > right) continue;
            
            int row_coord = row * tile_size - offset.y;
            int col_coord = column * tile_size - offset.x;
            SDL_Rect ground_tile = {col_coord, row_coord, tile_size, tile_size};

            if (map[row][column] != 0) {
                SDL_RenderCopy(renderer, ground_tex, nullptr, &ground_tile);
            }
        }
    }

    // Pass 2: Render all tree tiles w ground underneith
    for (int y = 0; y < map_size; y++) {
        if (y < top || y > bottom) continue;

        for (int x = 0; x < map_size; x++) {
            if (x < left || x > right) continue;

            if (map[y][x] == 2) {
                int row_coord = y * tile_size - offset.y;
                int col_coord = x * tile_size - offset.x;

                SDL_Rect ground_tile = {col_coord, row_coord, tile_size, tile_size};
                SDL_RenderCopy(renderer, ground_tex, nullptr, &ground_tile);
                SDL_Rect tree_tile = {
                    col_coord - (tile_size / 2),
                    row_coord - (tile_size * 2),
                    tile_size * 2,
                    tile_size * 3
                };
                SDL_RenderCopy(renderer, tree_tex, nullptr, &tree_tile);
            }
        }
    }
}


int random_number_gen(int size) {
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

    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            float dx = y - center_x;
            float dy = x - center_y;
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
                map[y][x] = land_val;
            } else {
                // Water
                map[y][x] = min_val;
            }
        }
    }
}


void print_map(int map[map_size][map_size]) {
    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            std::cout << map[y][x] << ' ';
        }
        std::cout << '\n';
    }std::cout << std::endl;
}


void update_offset(struct Offset& offset, int win_width, int win_height) {    

    offset.x = player.x - win_width / 2;
    offset.y = player.y - win_height / 2;

    // std::cout << "OFFSET: " << offset.x << ' ' << offset.y << '\n';
}


void update_player(struct Offset& offset, const Uint8* state) {
    /* see updateimine toimub ainult playeri liikumise pealt */
    int dx = 0;
    int dy = 0;

    if (state[SDL_SCANCODE_W]) dy -= 1;
    if (state[SDL_SCANCODE_S]) dy += 1;
    if (state[SDL_SCANCODE_A]) dx -= 1;
    if (state[SDL_SCANCODE_D]) dx += 1;

    // Normalize ? input == 2 : default movement logic
    float length = std::sqrt(dx * dx + dy * dy);
    if (length != 0) {
        dx = dx / length * player.movement_speed;
        dy = dy / length * player.movement_speed;

        player.x += dx;
        player.y += dy;
    }


    // std::cout << "PLAYER: " << player.x << ' ' << player.y << '\n';

}