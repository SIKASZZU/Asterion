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
#include <random>
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
    rect{ pos.x, pos.y, size, size },
    activity{ "roaming" },
    lastKnownPlayerGrid{ gx, gy },
    targetGrid{ gx, gy },
    roamingDistanceTraveled(0.0f),
    hasRoamingTarget(false)
{
}

// // void Enemy::activity() {
// //     if detection(player)
// //     else e.activity = "roaming";
// // }

// void Enemy::choose_action() {

//     // based on activity
//     switch (activity) {
//         case "roaming": {
//         }
//         case "chasing": {

//         }

//     }
// }

void Enemy::draw_path(const std::vector<std::pair<int, int>>& path) {
    // drawing path doesnt reset so it just stacks up error cubes :)
    for (const auto& p : path) {
        map[p.second][p.first] = Map::ERROR_CUBE;
    }
}

void Enemy::update(const int map[mapSize][mapSize], SDL_Point targetGrid, float dT) {

    if (stopAllEnemies) return;
    choose_target(map, targetGrid);
    if (path.empty() || currentPathIndex >= path.size() ||
        (path.back().first != this->targetGrid.x || path.back().second != this->targetGrid.y)) {
        if (!is_walkable(map, this->targetGrid)) { std::cout << "not walkable" << '\n'; return; };
        compute_path(map, this->targetGrid);
        // draw_path(path);
    }
    move_along_path(dT);
    // If roaming and reached target but haven't traveled enough, choose new
    if (strcmp(activity, "roaming") == 0 && grid.x == this->targetGrid.x && grid.y == this->targetGrid.y && roamingDistanceTraveled < 2 * tileSize) {
        hasRoamingTarget = false;
        choose_target(map, targetGrid);
    }
}
void Enemy::render(SDL_Renderer* renderer) {
    SDL_FPoint isoPos = to_isometric_coordinate(pos.x, pos.y);
    rect = { isoPos.x, isoPos.y - (tileSize / 4), size, size };
    animation(renderer, activity);
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
    if (strcmp(activity, "roaming") == 0) {
        roamingDistanceTraveled += moveAmount;
    }
}

bool Enemy::is_walkable(const int map[mapSize][mapSize], SDL_Point targetGrid) {
    if (targetGrid.y < 0 || targetGrid.y >= mapSize || targetGrid.x < 0 || targetGrid.x >= mapSize) return false;
    return wallValues.find(map[targetGrid.y][targetGrid.x]) == wallValues.end();
}

// Simple line of sight check using Bresenham's line algorithm
bool Enemy::has_line_of_sight(const int map[mapSize][mapSize], SDL_Point from, SDL_Point to) {
    int x0 = from.x, y0 = from.y;
    int x1 = to.x, y1 = to.y;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    int e2;

    while (true) {
        if (x0 == x1 && y0 == y1) break;
        // Check if current grid is a wall (but allow start and end)
        if ((x0 != from.x || y0 != from.y) && (x0 != to.x || y0 != to.y)) {
            if (wallValues.find(map[y0][x0]) != wallValues.end()) {
                return false;
            }
        }
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
    return true;
}

void Enemy::choose_target(const int map[mapSize][mapSize], SDL_Point playerGrid) {
    float dist = std::hypot(pos.x - player.x, pos.y - player.y);
    if (dist <= 500.0f) {
        lastKnownPlayerGrid = playerGrid;
        targetGrid = playerGrid;
        hasRoamingTarget = false;

        // attack range
        int attackRange = tileSize;
        if (dist <= attackRange) {
            activity = "idle";
        }
        else {
            activity = "chasing";
        }

    }
    else if (strcmp(activity, "chasing") == 0) {
        // Continue to last known if no line of sight or not in range
        if (has_line_of_sight(map, grid, playerGrid)) {
            targetGrid = playerGrid;
        }
        else {
            targetGrid = lastKnownPlayerGrid;
            if (grid.x == lastKnownPlayerGrid.x && grid.y == lastKnownPlayerGrid.y) {
                activity = "roaming";
                hasRoamingTarget = false;
            }
        }
    }
    else {
        // Roaming
        if (!hasRoamingTarget || roamingDistanceTraveled >= 2 * tileSize) {
            // Choose new direction
            std::vector<SDL_Point> directions = {
                {0, 1}, {1, 0}, {0, -1}, {-1, 0},
                {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
            };
            std::shuffle(directions.begin(), directions.end(), std::mt19937{ std::random_device{}() });
            for (auto& dir : directions) {
                SDL_Point newGrid = { grid.x + dir.x, grid.y + dir.y };
                if (is_walkable(map, newGrid)) {
                    targetGrid = newGrid;
                    hasRoamingTarget = true;
                    roamingDistanceTraveled = 0.0f;
                    return;
                }
            }
            // If no direction works, stay
            targetGrid = grid;
            hasRoamingTarget = true;
            roamingDistanceTraveled = 0.0f;
        }
    }
}
