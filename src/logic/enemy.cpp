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

// Enemy movement inertia constants
const float ENEMY_ACCEL = 1500.0f;
const float ENEMY_FRICTION = 1200.0f;

bool stopAllEnemies = false;
std::vector<Enemy> enemyArray = {};
const int maxRoamingDistance = 5 * tileSize;
bool debugText = false;

Enemy::Enemy(int gx, int gy)
    : grid{ gx, gy },
    size(tileSize),
    color({ 255, 0, 0, 255 }),
    currentPathIndex(0),
    movementVector{ 0, 0 },
    speed(player.movementSpeed),
    velocity{ 0.0f, 0.0f },
    pos{
        static_cast<float>(grid.x * tileSize + (size / 2)),
        static_cast<float>(grid.y * tileSize + (size / 2))
    },
    rect{ pos.x, pos.y, size, size },
    lastKnownPlayerGrid{ gx, gy },
    targetGrid{ gx, gy },
    roamingDistanceTraveled(0.0f),
    hasRoamingTarget(false),
    standingTimer(0.0f),
    state{ EnemyState::Idle },
    activity{ EnemyActivity::Roam },
    anim_lastUpdate(SDL_GetTicks()),
    anim_currentAnimCol(0),
    anim_currentAnimRow(0),
    anim_spriteEnum(0),
    anim_previousState(0)
{
}

const char* stateToString(EnemyState s) {
    switch (s) {
    case (EnemyState::Idle): return "Idle"; break;
    case (EnemyState::Walk): return "Walk"; break;
    case (EnemyState::Run): return "Run"; break;
    case (EnemyState::Dead): return "Dead"; break;
    case (EnemyState::Raise): return "Raise"; break;
    default: return "Unconfigured state"; break;
    }
}

const char* activityToString(EnemyActivity a) {
    switch (a) {
    case (EnemyActivity::Idle): return "Idle"; break;
    case (EnemyActivity::Roam): return "Roam"; break;
    case (EnemyActivity::Chase): return "Chase"; break;
    case (EnemyActivity::Raise): return "Raise"; break;
    default: return "Unconfigured activity"; break;
    }
}

void Enemy::choose_state() {
    switch (activity) {
    case EnemyActivity::Idle: {
        state = EnemyState::Idle;
        break;
    }
    case EnemyActivity::Roam: {
        state = EnemyState::Walk;
        break;
    }
    case EnemyActivity::Chase: {
        state = EnemyState::Run;
        break;
    }
    case EnemyActivity::Raise: {
        state = EnemyState::Raise;
        break;
    }
    }
}

void Enemy::choose_activity(SDL_Point tG) {
    // Calculate distance in pixels for precision
    float dist = std::hypot(pos.x - (tG.x * tileSize + tileSize / 2),
        pos.y - (tG.y * tileSize + tileSize / 2));

    // Detection Range: 5 tiles
    if (dist <= tileSize * 5) {
        // if (has_line_of_sight(map, Enemy::grid, tG)) {
        lastKnownPlayerGrid = tG;
        activity = EnemyActivity::Chase;
        targetGrid = lastKnownPlayerGrid;
        // }
    }

    // Attack Range Check
    if (activity == EnemyActivity::Chase) {
        if (dist <= tileSize * 0.8f) { // Within 80% of a tile
            activity = EnemyActivity::Idle;
        }
    }

    // If we were chasing but reached the last known location and player is gone
    if (activity == EnemyActivity::Chase && grid.x == lastKnownPlayerGrid.x && grid.y == lastKnownPlayerGrid.y) {
        activity = EnemyActivity::Roam;
        hasRoamingTarget = false;
    }
    if (spawning == true) {
        activity = EnemyActivity::Raise;
    }

    if (activity == EnemyActivity::Raise && spawning == false) {
        activity = EnemyActivity::Roam;
    }
}

void Enemy::compute_path(const int map[mapSize][mapSize], SDL_Point targetGrid) {
    path.clear();
    currentPathIndex = 0;
    bool pathFound = Maze::find_path(map, grid.x, grid.y, targetGrid.x, targetGrid.y);
    if (pathFound) path = Maze::path;
}

bool Enemy::is_walkable(const int map[mapSize][mapSize], SDL_Point targetGrid) {
    if (targetGrid.y < 0 || targetGrid.y >= mapSize || targetGrid.x < 0 || targetGrid.x >= mapSize) return false;
    return wallValues.find(map[targetGrid.y][targetGrid.x]) == wallValues.end();
}

void Enemy::move_along_path(float dT) {
    if (path.empty() || currentPathIndex >= path.size()) {
        // Reset movement if no path
        movementVector = { 0, 0 };
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

    if (dist <= 4.0f) {
        // Snap precisely to the grid cell
        pos = nextPos;
        grid = { nextX, nextY };
        currentPathIndex++;
        return;
    }

    // Move by already-calculated velocity (with delta time)
    SDL_FPoint moveBy = { velocity.x * dT, velocity.y * dT };
    pos.x += moveBy.x;
    pos.y += moveBy.y;

    if (activity == EnemyActivity::Roam) {
        roamingDistanceTraveled += std::hypot(moveBy.x, moveBy.y);
    }

    movementVector = { nextGrid.first - grid.x, nextGrid.second - grid.y };
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

// Calculate velocity (inertia) towards the next path node.
void Enemy::calculate_velocity(float dT) {

    if (path.empty() || currentPathIndex >= path.size()) {
        // No target -> apply friction to slow down
        if (velocity.x > 0)
            velocity.x = std::max(0.0f, velocity.x - ENEMY_FRICTION * dT);
        else if (velocity.x < 0)
            velocity.x = std::min(0.0f, velocity.x + ENEMY_FRICTION * dT);

        if (velocity.y > 0)
            velocity.y = std::max(0.0f, velocity.y - ENEMY_FRICTION * dT);
        else if (velocity.y < 0)
            velocity.y = std::min(0.0f, velocity.y + ENEMY_FRICTION * dT);

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

    float dirX = 0.0f;
    float dirY = 0.0f;
    if (dist > 1e-6f) {
        dirX = dx / dist;
        dirY = dy / dist;
    }

    // Apply acceleration towards movement direction
    velocity.x += dirX * ENEMY_ACCEL * dT;
    velocity.y += dirY * ENEMY_ACCEL * dT;

    // Apply friction when there is effectively no intended movement along an axis
    if (std::abs(dirX) < 1e-6f) {
        if (velocity.x > 0)
            velocity.x = std::max(0.0f, velocity.x - ENEMY_FRICTION * dT);
        else if (velocity.x < 0)
            velocity.x = std::min(0.0f, velocity.x + ENEMY_FRICTION * dT);
    }
    if (std::abs(dirY) < 1e-6f) {
        if (velocity.y > 0)
            velocity.y = std::max(0.0f, velocity.y - ENEMY_FRICTION * dT);
        else if (velocity.y < 0)
            velocity.y = std::min(0.0f, velocity.y + ENEMY_FRICTION * dT);
    }

    // If we're very close to the current path node, prepare velocity towards
    // the following node (if any) so movement doesn't snap or jitter per-tile.
    if (!path.empty() && currentPathIndex < path.size()) {
        const auto& nextGrid = path[currentPathIndex];
        SDL_FPoint nextPos{
            static_cast<float>((nextGrid.first * tileSize + tileSize / 2)),
            static_cast<float>((nextGrid.second * tileSize + tileSize / 2))
        };
        float ndx_toNode = nextPos.x - pos.x;
        float ndy_toNode = nextPos.y - pos.y;
        float nodeDist = std::hypot(ndx_toNode, ndy_toNode);

        if (nodeDist < 4.0f) {
            // If there's a following node, accelerate towards it
            if (currentPathIndex + 1 < path.size()) {
                const auto& futureGrid = path[currentPathIndex + 1];
                SDL_FPoint futurePos{
                    static_cast<float>((futureGrid.first * tileSize + tileSize / 2)),
                    static_cast<float>((futureGrid.second * tileSize + tileSize / 2))
                };
                float fdx = futurePos.x - pos.x;
                float fdy = futurePos.y - pos.y;
                float fdist = std::hypot(fdx, fdy);
                if (fdist > 1e-6f) {
                    float desiredX = fdx / fdist;
                    float desiredY = fdy / fdist;
                    float curSpeed = std::hypot(velocity.x, velocity.y);
                    float accelStep = ENEMY_ACCEL * dT;
                    float targetSpeed = std::min(curSpeed + accelStep, speed);
                    if (targetSpeed < 1e-3f) targetSpeed = std::min(accelStep, speed * 0.25f);
                    velocity.x = desiredX * targetSpeed;
                    velocity.y = desiredY * targetSpeed;
                }
            }
            // else: at the final node, leave velocity for friction to handle
        }
    }

    // Clamp velocity magnitude to speed
    float vmag = std::hypot(velocity.x, velocity.y);
    if (vmag > speed && vmag > 0.0f) {
        velocity.x = (velocity.x / vmag) * speed;
        velocity.y = (velocity.y / vmag) * speed;
    }
}

void Enemy::choose_target(const int map[mapSize][mapSize], SDL_Point tG) {

    // IF CHASING: The target is already set to the player/last known by choose_activity.
    // We just need to exit so we don't overwrite it with roaming logic.
    if (activity == EnemyActivity::Chase) {
        hasRoamingTarget = false;
        return;
    }

    // ROAMING LOGIC
    bool roamComplete = roamingDistanceTraveled >= maxRoamingDistance;
    if (roamComplete) {
        hasRoamingTarget = false;
    }

    if (!hasRoamingTarget) {
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
                standingTimer = 0.0f;
                return;
            }

        }
        targetGrid = grid;
        hasRoamingTarget = true;
    }
}

void Enemy::render(SDL_Renderer* renderer) {
    SDL_FPoint isoPos = to_isometric_coordinate(pos.x, pos.y);
    rect = { isoPos.x + offset.x, isoPos.y + offset.y - (tileSize / 4), size, size };

    animation(renderer);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderRect(renderer, &rect);
    if (debugText) {
        debug(renderer);
    }

}

void Enemy::update(const int map[mapSize][mapSize], SDL_Point playerGrid, float dT) {
    if (stopAllEnemies) return;

    choose_activity(playerGrid);
    choose_state();
    choose_target(map, playerGrid);

    if (activity == EnemyActivity::Raise) {
        velocity = SDL_FPoint{ 0,0 };
        return;
    }

    bool needsNewPath = path.empty() || currentPathIndex >= path.size();
    if (needsNewPath) {
        if (is_walkable(map, targetGrid)) {
            compute_path(map, targetGrid);
        }
    }

    // 2. IDLE TIMER (Only for roaming)
    if (activity == EnemyActivity::Roam && standingTimer < 1.5f) {
        standingTimer += dT;
        state = EnemyState::Idle;
        return;
    }

    // calculate velocity (inertia) first, then move using that velocity
    calculate_velocity(dT);
    move_along_path(dT);

    // 3. ROAMING SUCCESSION
    if (activity == EnemyActivity::Roam && grid.x == targetGrid.x && grid.y == targetGrid.y) {
        if (roamingDistanceTraveled < maxRoamingDistance) {
            hasRoamingTarget = false; // Trigger new roam target next frame
        }
    }
}

void Enemy::debug(SDL_Renderer* renderer) {
    // using namespace Enemy;
    // SDL_SetRenderDrawColor(renderer, 44, 62, 80, 255);
    SDL_SetRenderDrawColor(renderer, 241, 196, 15, 255);

    int lineHeight = 10;
    int x = (Enemy::rect.x + Enemy::rect.w) / 1.5; int y = Enemy::rect.y / 1.5;

    auto drawLine = [&](const std::string& text) {
        SDL_RenderDebugText(renderer, x, y, text.c_str());
        y += lineHeight;
        };
    SDL_SetRenderScale(renderer, 1.5f, 1.5f);

    drawLine("activity: " + std::string(activityToString(activity)));
    drawLine("state:    " + std::string(stateToString(state)));
    drawLine("mVector:  " + std::to_string(movementVector.x) + " " + std::to_string(movementVector.y));
    drawLine("velocity:  " + std::to_string(Enemy::velocity.x) + " " + std::to_string(velocity.y));
    drawLine("spawning:  " + std::string(Enemy::spawning ? "True" : "False"));

    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}
