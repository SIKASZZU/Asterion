#pragma once
#include "game.h"

struct Player {
    float movement_speed;
    int size;
    float x;
    float y;
    double direction;
    SDL_FRect rect;
};

extern Player player;

void update_player(int map[map_size][map_size], struct Offset& offset, const Uint8* state, SDL_Renderer* renderer);
void draw_player(SDL_Renderer* renderer, struct Player& player, struct Offset& offset);

double calculate_2_point_distance(double x1, double y1, double x2, double y2);
void update_player_direction(struct Player& player, SDL_Window* window, struct Offset& offset, int mouse_x, int mouse_y);
void draw_player_direction(SDL_Renderer* renderer, struct Player& player, struct Offset& offset, int mouse_x, int mouse_y);