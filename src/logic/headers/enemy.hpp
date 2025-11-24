#pragma once
#include <vector>
#include <SDL3/SDL.h>

#include "offset.hpp"
#include "game.hpp"

class Enemy {
public:
    Enemy(int gridX, int gridY);
    SDL_Point grid;
    
    void animation(SDL_Renderer* renderer);
    void update(const int map[mapSize][mapSize], SDL_Point targetGrid, float dT);
    void render(SDL_Renderer* renderer);
    void set_speed(float s) { speed = s; };
    void set_size(float sz) { size = sz; };
    void set_position(SDL_FPoint p) { pos = p; };
    SDL_FPoint get_position() const { return pos; };
    SDL_Point get_movementVector() const { return movementVector; };
    SDL_FRect get_rect() const {return rect; };
    int get_size() const {return size; };

private:
    float size;
    float speed;
    std::vector<std::pair<int, int>> path;
    SDL_FPoint pos;
    SDL_FRect rect;
    SDL_Color color;
    size_t currentPathIndex;
    SDL_Point movementVector;

    void move_along_path(float dT);
    void compute_path(const int map[mapSize][mapSize], SDL_Point targetGrid);
    bool is_walkable(const int map[mapSize][mapSize], SDL_Point targetGrid);
    void draw_path(const std::vector<std::pair<int, int>>& path);

};

extern bool stopAllEnemies;
extern std::vector<Enemy> enemyArray;
