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

#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

Enemy::Enemy(int gx, int gy, int tile_size)
    : grid{gx, gy}, size(tile_size / 2), speed(DEFAULT_MOVEMENT_SPEED), color({ 255, 0, 0, 255 }),
    currentPathIndex(0)
{
    pos = { static_cast<float>(grid.x * tile_size), static_cast<float>(grid.y * tile_size) };
}

void Enemy::update(const int map[map_size][map_size], SDL_Point target) {
    SDL_Point targetGrid = { static_cast<int>(target.x / tile_size), static_cast<int>(target.y / tile_size) };
    if (path.empty() || currentPathIndex >= (int)path.size()) {
        if (!is_walkable(map, targetGrid)) {std::cout << "not walkable" << '\n'; return; };
        compute_path(map, targetGrid);
    }
    move_along_path(targetGrid);
}

void Enemy::render(SDL_Renderer* renderer, struct Offset& offset) {
    SDL_FPoint isoPos = to_isometric_coordinate(offset, pos.x, pos.y);
    SDL_FRect rect = { isoPos.x, isoPos.y, static_cast<float>(size), static_cast<float>(size) };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// -------------------- Private ----------------------

void Enemy::compute_path(const int map[map_size][map_size], SDL_Point targetGrid) {
    path.clear();
    currentPathIndex = 0;
    Maze::find_path(map, grid.x, grid.y, targetGrid.x, targetGrid.y);
    path = Maze::path;
}

void Enemy::move_along_path(SDL_Point targetGrid) {
    // Stop if there’s no path or we already reached the end
    if (path.empty() || currentPathIndex >= static_cast<int>(path.size())) {
        return;
    }

    // Grid coordinates of the next waypoint
    const auto& nextGrid = path[currentPathIndex];
    int nextX = nextGrid.first;
    int nextY = nextGrid.second;

    // World-space target position for the current waypoint
    float halfTile = tile_size * 0.5f;
    SDL_FPoint nextPos {
        static_cast<float>((nextX * tile_size) + halfTile),
        static_cast<float>((nextY * tile_size) + halfTile)
    };
    float dx = nextPos.x - pos.x;
    float dy = nextPos.y - pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < tile_size) {
        grid.x = nextX;
        grid.y = nextY;
        currentPathIndex++;
    }
    else {
        pos.x += (dx / dist) * speed;
        pos.y += (dy / dist) * speed;
    }
}

bool Enemy::is_walkable(const int map[map_size][map_size], SDL_Point targetGrid) {
    return wall_values.find(map[targetGrid.x][targetGrid.y]) == wall_values.end();
}