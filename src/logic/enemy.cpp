// 45, 225, 135, 315

// Walk_Body_045.png
// Run_Body_0135.png

// if dir.y = -1; angle = 45
// if dir.y = 1; angle = 225
// if dir.x = 1; angle = 135
// if dir.x = -1; angle = 315

#include "enemy.hpp"
#include "game.hpp"
#include "player.hpp"
#include "isometric_calc.hpp"
#include "maze.hpp"
#include "textures.hpp"

#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

bool stopAllEnemies = false;
std::vector<Enemy> enemyArray = {};

Enemy::Enemy(int gx, int gy)
    :   grid{ gx, gy },
        size(tileSize),
        color({ 255, 0, 0, 255 }),
        currentPathIndex(0),
        movementVector{ 0, 0 },
        speed(player.movementSpeed),
        pos {
            static_cast<float>(grid.x * tileSize),
            static_cast<float>(grid.y * tileSize)
        },
        rect{ pos.x, pos.y, size, size }
{ }

void Enemy::update(const int map[mapSize][mapSize], SDL_Point targetGrid, float dT) {

    if (stopAllEnemies) return;
    if (path.empty() || currentPathIndex >= path.size()) {
        if (!is_walkable(map, targetGrid)) { std::cout << "not walkable" << '\n'; return; };
        compute_path(map, targetGrid);
    }
    move_along_path(dT);
}

void Enemy::render(SDL_Renderer* renderer, struct Offset& offset) {
    SDL_FPoint isoPos = to_isometric_coordinate(pos.x, pos.y);
    rect = { isoPos.x, isoPos.y - size / 4, size, size };
    animation(renderer);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderRect(renderer, &rect);
}

// -------------------- Private ----------------------

void Enemy::compute_path(const int map[mapSize][mapSize], SDL_Point targetGrid) {
    path.clear();
    currentPathIndex = 0;
    Maze::find_path(map, grid.x, grid.y, targetGrid.x, targetGrid.y);
    path = Maze::path;
}

void Enemy::move_along_path(float dT) {
    if (path.empty()) {
        return;
    }

    const auto& nextGrid = path[currentPathIndex];
    int nextX = nextGrid.first;
    int nextY = nextGrid.second;
    SDL_FPoint nextPos{
        static_cast<float>((nextX * tileSize + tileSize / 2)),
        static_cast<float>((nextY * tileSize + tileSize / 2))
    };
    float dx = nextPos.x - pos.x;
    float dy = nextPos.y - pos.y;
    float dist = std::hypot(dx, dy);

    // Update movement direction for animation (45°, 135°, etc.)
    // todo: see sitt ei lenda, kui tahan koordinaatidega tegema hakata? mingi skalaarvektor vmdgi playeri suunas, j2rgmise pathi ja korras?
    movementVector = { nextX - grid.x, nextY - grid.y };
    if (dist <= 4.0f) {
        pos = nextPos;
        grid = { nextX, nextY };
        currentPathIndex++;
        return;
    }

    float moveAmount = speed; // * dT
    pos.x += (dx / dist) * moveAmount;
    pos.y += (dy / dist) * moveAmount;
}

bool Enemy::is_walkable(const int map[mapSize][mapSize], SDL_Point targetGrid) {
    return wallValues.find(map[targetGrid.x][targetGrid.y]) == wallValues.end();
}
