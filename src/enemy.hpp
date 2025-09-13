#pragma once
#include <vector>
#include <SDL3/SDL.h>

#include "game.hpp"

class Enemy {
    public:
        Enemy(int gridX, int gridY, int size);

        void update(const int map[map_size][map_size], SDL_Point targetGrid);
        void render(SDL_Renderer* renderer, struct Offset& offset);

        void set_speed(float s) { speed = s; }
        void set_color(SDL_Color c) { color = c; }
        SDL_FPoint get_position() const { return pos; }

    private:
        SDL_FPoint pos;
        SDL_Point grid;
        int size;
        float speed;
        SDL_Color color;

        std::vector<std::pair<int, int>> path;
        int currentPathIndex;

        void move_along_path(SDL_Point targetGrid);
        void compute_path(const int map[map_size][map_size], SDL_Point targetGrid);

        bool is_walkable(const int map[map_size][map_size], SDL_Point targetGrid);
};