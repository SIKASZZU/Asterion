#pragma once
#include <vector>
#include <SDL3/SDL.h>

#include "offset.hpp"
#include "game.hpp"

enum class EnemyState {
    Idle,
    Walk,
    Run,
    Dead,
};


enum class EnemyActivity {
    Idle,
    Roam,
    Chase,
};

const char* stateToString(EnemyState s);
const char* activityToString(EnemyActivity a);

extern bool debugText;

class Enemy {
public:
    Enemy(int gridX, int gridY);
    SDL_Point grid;
    SDL_Point lastDirection;

    void animation(SDL_Renderer* renderer);
    void update(const int map[mapSize][mapSize], SDL_Point targetGrid, float dT);
    void render(SDL_Renderer* renderer);
    void set_speed(float s) { speed = s; };
    void set_size(float sz) { size = sz; };
    void set_position(SDL_FPoint p) { pos = p; };
    SDL_FPoint get_position() const { return pos; };
    SDL_Point get_movementVector() const { return movementVector; };
    SDL_FRect get_rect() const { return rect; };
    int get_size() const { return size; };
    EnemyState state;
    EnemyActivity activity;

private:
    float size;
    float speed;
    std::vector<std::pair<int, int>> path;
    SDL_FPoint pos;
    SDL_FPoint velocity;
    SDL_FRect rect;
    SDL_Color color;
    size_t currentPathIndex;
    SDL_Point movementVector;
    SDL_Point lastKnownPlayerGrid;
    SDL_Point targetGrid;
    float roamingDistanceTraveled;
    bool hasRoamingTarget;
    float standingTimer;

    void choose_state();
    void choose_activity(SDL_Point tG);
    void calculate_velocity(float dT);
    void move_along_path(float dT);
    void compute_path(const int map[mapSize][mapSize], SDL_Point targetGrid);
    bool is_walkable(const int map[mapSize][mapSize], SDL_Point targetGrid);
    void choose_target(const int map[mapSize][mapSize], SDL_Point playerGrid);
    bool has_line_of_sight(const int map[mapSize][mapSize], SDL_Point from, SDL_Point to);
    void debug(SDL_Renderer* renderer);
};

extern bool stopAllEnemies;
extern std::vector<Enemy> enemyArray;