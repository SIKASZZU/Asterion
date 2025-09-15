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

bool stopAllEnemies = true;
Uint32 lastUpdate = SDL_GetTicks();
int lastFrame = 0;
int animRow = 0;
int animCol = 0;
std::vector<Enemy> enemyArray = {};

Enemy::Enemy(int gx, int gy, int tile_size)
    : grid{gx, gy}, size(tile_size * 1), rect{pos.x, pos.y, size, size}, speed(player.movement_speed), color({ 255, 0, 0, 255 }),
    currentPathIndex(0), movementVector{0,0}
{
    pos = { static_cast<float>(grid.x * tile_size), static_cast<float>(grid.y * tile_size) };
}

void Enemy::update(const int map[map_size][map_size], SDL_Point target) {
    SDL_Point targetGrid = { static_cast<int>(target.x / tile_size), static_cast<int>(target.y / tile_size) };
    
    if (stopAllEnemies) return;
    if (path.empty()) {
        if (!is_walkable(map, targetGrid)) {std::cout << "not walkable" << '\n'; return; };
        compute_path(map, targetGrid);
    }
    move_along_path(targetGrid);
}

void Enemy::render(SDL_Renderer* renderer, struct Offset& offset) {
    SDL_FPoint isoPos = to_isometric_coordinate(offset, pos.x, pos.y);
    rect = { isoPos.x, isoPos.y - size / 4, size, size };
    animation(renderer);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderRect(renderer, &rect);
}

// -------------------- Private ----------------------

void Enemy::compute_path(const int map[map_size][map_size], SDL_Point targetGrid) {
    path.clear();
    currentPathIndex = 0;
    Maze::find_path(map, grid.x, grid.y, targetGrid.x, targetGrid.y);
    path = Maze::path;
}

void Enemy::move_along_path(SDL_Point targetGrid) {
    if (path.empty()) {
        return;
    }

    const auto& nextGrid = path.front();
    int nextX = nextGrid.first;
    int nextY = nextGrid.second;

    SDL_FPoint nextPos {
        static_cast<float>((nextX * tile_size)),
        static_cast<float>((nextY * tile_size))
    };
    float dx = nextPos.x - pos.x;
    float dy = nextPos.y - pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    movementVector = { nextX - grid.x, nextY - grid.y };

    if (dist <= tile_size) {
        grid.x = nextX;
        grid.y = nextY;
        path.erase(path.begin());
    }
    else {
        pos.x += (dx / dist) * speed;
        pos.y += (dy / dist) * speed;
    }
}

bool Enemy::is_walkable(const int map[map_size][map_size], SDL_Point targetGrid) {
    return wall_values.find(map[targetGrid.x][targetGrid.y]) == wall_values.end();
}

void Enemy::animation(SDL_Renderer* renderer) {
    const int sprite_width = 256;
    const int sprite_height = 256;
    SDL_FRect srcRect;
    int animationSpeed = 18;
    animCol = last_frame % 16;

    if (movementVector.x == 1) { animRow = 1; }
    if (movementVector.x == -1) { animRow = 3; }

    if (movementVector.y == 1) { animRow = 2; }
    if (movementVector.y == -1) { animRow = 0; }

    srcRect.x = animCol * sprite_width;
    srcRect.y = animRow * sprite_height;
    srcRect.w = sprite_width;
    srcRect.h = sprite_height;

    SDL_FRect dstRect = rect;
    float scaleFactor = 1.0f;
    dstRect.w = sprite_width * scaleFactor;
    dstRect.h = sprite_height * scaleFactor;
    // center inside the original rect (e.g. tile)
    dstRect.x = rect.x + (rect.w - dstRect.w) * 0.5f;
    dstRect.y = rect.y + (rect.h - dstRect.h) * 0.5f;

    if (SDL_GetTicks() - lastUpdate > animationSpeed) {
        lastFrame++;
        lastUpdate = SDL_GetTicks();
    }
    texture_map[Map::SPIDER].render(renderer, &srcRect, &dstRect);
}