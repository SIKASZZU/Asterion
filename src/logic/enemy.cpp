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
    : grid{ gx, gy },
    size(tileSize),
    color({ 255, 0, 0, 255 }),
    currentPathIndex(0),
    movementVector{ 0, 0 },
    speed(player.movementSpeed),
    pos{
        static_cast<float>(grid.x * tileSize),
        static_cast<float>(grid.y * tileSize)
    },
    rect{ pos.x, pos.y, size, size }
{
}

void Enemy::draw_path(const std::vector<std::pair<int, int>>& path) {
    // drawing path doesnt reset so it just stacks up error cubes :)
    for (const auto& p : path) {
        map[p.second][p.first] = Map::ERROR_CUBE;
    }
}

void Enemy::update(const int map[mapSize][mapSize], SDL_Point targetGrid, float dT) {

    if (stopAllEnemies) return;
    if (path.empty() || currentPathIndex >= path.size() ||
        (path.back().first != targetGrid.x || path.back().second != targetGrid.y) && player.movementSpeed == 0) {
        if (!is_walkable(map, targetGrid)) { std::cout << "not walkable" << '\n'; return; };
        compute_path(map, targetGrid);
        // draw_path(path);
    }
    move_along_path(dT);
}

void Enemy::render(SDL_Renderer* renderer) {
    SDL_FPoint isoPos = to_isometric_coordinate(pos.x, pos.y);
    rect = { isoPos.x, isoPos.y - (tileSize / 4), size, size };
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
    // for (auto& p : path) {
    //     map[p.second][p.first] = Map::ERROR_CUBE;
    // }
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

    movementVector = { nextX - grid.x, nextY - grid.y };
    {
        // use the grid delta so discrete movement aligns with path steps
        float nx = static_cast<float>(nextX - grid.x);
        float ny = static_cast<float>(nextY - grid.y);
        int qx = 0, qy = 0;
        if (nx != 0.0f || ny != 0.0f) {
            // invert ny for screen/grid orientation so "up" becomes negative y in mapping
            float angle = std::atan2(-ny, nx); // angle in radians
            // map angle to closest 45° sector (8-way)
            int sector = static_cast<int>(std::round(angle / (PI / 4.0f)));
            sector = (sector % 8 + 8) % 8; // normalize to [0,7]
            switch (sector) {
            case 0: qx = 1; qy = 0; break; // E
            case 1: qx = 1; qy = -1; break; // NE
            case 2: qx = 0; qy = -1; break; // N
            case 3: qx = -1; qy = -1; break; // NW
            case 4: qx = -1; qy = 0; break; // W
            case 5: qx = -1; qy = 1; break; // SW
            case 6: qx = 0; qy = 1; break; // S
            case 7: qx = 1; qy = 1; break; // SE
            }
        }
        movementVector = { qx, qy };
    }
    if (dist <= 4.0f) {
        pos = nextPos;
        grid = { nextX, nextY };
        currentPathIndex++;
        return;
    }

    float moveAmount = speed * dT;
    pos.x += (dx / dist) * moveAmount;
    pos.y += (dy / dist) * moveAmount;
}

bool Enemy::is_walkable(const int map[mapSize][mapSize], SDL_Point targetGrid) {
    if (targetGrid.y < 0 || targetGrid.y >= mapSize || targetGrid.x < 0 || targetGrid.x >= mapSize) return false;
    return wallValues.find(map[targetGrid.y][targetGrid.x]) == wallValues.end();
}
