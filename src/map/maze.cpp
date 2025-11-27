#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <random>
#include <queue>
#include <utility>
#include <unordered_map>

#include "map.hpp"
#include "game.hpp"

namespace Maze {
    // Directions: up, down, left, right
    std::vector<std::pair<int, int>> directions_sec_3 = {
        {-4, 0}, {4, 0}, {0, -4}, {0, 4}  // suuremad vahed
    };

    std::vector<std::pair<int, int>> directions_sec_2 = {
        {-4, 0}, {2, 0}, {0, -4}, {0, 2}  // perse l2inud symbioos
    };

    std::vector<std::pair<int, int>> directions_sec_1 = {
        {-2, 0}, {2, 0}, {0, -2}, {0, 2}  // normal maze
    };

    std::vector<std::pair<int, int>> path;
    int pathway = Map::MAZE_GROUND_CUBE;

    // Shuffle directions
    void shuffle_directions(std::vector<std::pair<int, int>>& directions) {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(directions.begin(), directions.end(), g);
    }
    // Recursive maze generation using DFS
    void generate_maze(int map[mapSize][mapSize], int start_row, int start_col, std::string type) {
        std::vector<std::pair<int, int>> directions;
        int allowed_number;

        if (type == "one") {
            directions = directions_sec_1;
            allowed_number = Map::SECTOR_1_WALL_VAL;
        }
        else if (type == "two") {
            directions = directions_sec_2;
            allowed_number = Map::SECTOR_2_WALL_VAL;
        }
        else if (type == "three") {
            directions = directions_sec_3;
            allowed_number = Map::SECTOR_3_WALL_VAL;
        }

        shuffle_directions(directions);

        for (const auto& dir : directions) {
            // start_row/start_col correspond to map[row][col]
            int nx = start_col + dir.first; // next x (column)
            int ny = start_row + dir.second; // next y (row)

            if (map[ny][nx] != allowed_number) {
                if (map[ny][nx] == pathway && type != "one") {
                    continue;
                }
                continue;
            }

            if (nx > 0 && ny > 0 && nx < mapSize - 1 && ny < mapSize - 1) {
                map[ny][nx] = pathway;
                map[start_row + (dir.second != 0 ? dir.second / 2 : dir.second)][start_col + (dir.first != 0 ? dir.first / 2 : dir.first)] = pathway;

                if (type == "two" || type == "three") {
                    map[start_row + (dir.second != 0 ? dir.second + 2 : dir.second)][start_col + (dir.first != 0 ? dir.first + 2 : dir.first)] = pathway;
                    map[start_row + (dir.second != 0 ? dir.second + 3 : dir.second)][start_col + (dir.first != 0 ? dir.first + 3 : dir.first)] = pathway;
                    map[start_row + (dir.second != 0 ? dir.second + 1 : dir.second)][start_col + (dir.first != 0 ? dir.first + 1 : dir.first)] = pathway;
                }

                if (type == "three") {
                    map[start_row + (dir.second != 0 ? dir.second - 3 : dir.second)][start_col + (dir.first != 0 ? dir.first - 3 : dir.first)] = pathway;
                    map[start_row + (dir.second != 0 ? dir.second - 1 : dir.second)][start_col + (dir.first != 0 ? dir.first - 1 : dir.first)] = pathway;
                }

                generate_maze(map, ny, nx, type);
            }
        }
    }
    bool is_walkable(int gridValue) {
        return wallValues.find(gridValue) == wallValues.end();
    }
    // A* Pathfinding from (sx, sy) to (gx, gy) with 8-way movement
    bool find_path(const int map[mapSize][mapSize], int sx, int sy, int gx, int gy) {

        path.clear();

        using pii = std::pair<int, int>;
        std::unordered_map<pii, pii, pair_hash> came_from;

        // g-score: cost from start to this node
        const double INF = 1e12;
        static double g_score[mapSize][mapSize];
        for (int y = 0; y < mapSize; ++y)
            for (int x = 0; x < mapSize; ++x)
                g_score[y][x] = INF;

        auto heuristic = [&](int x, int y) {
            double dx = double(gx - x);
            double dy = double(gy - y);
            return std::sqrt(dx * dx + dy * dy);
            };

        // 8 neighbors: 4 cardinal + 4 diagonals
        int dxs[8] = { -1, 1, 0, 0, -1, -1, 1, 1 };
        int dys[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };

        // Priority queue ordered by f = g + h (min-heap via greater)
        using PQElem = std::pair<double, pii>;
        std::priority_queue<PQElem, std::vector<PQElem>, std::greater<PQElem>> open;

        g_score[sy][sx] = 0.0;
        open.push({ heuristic(sx, sy), { sx, sy } });

        while (!open.empty()) {
            auto [f, coord] = open.top(); open.pop();
            int x = coord.first, y = coord.second;

            // If we reached the goal, reconstruct path
            if (x == gx && y == gy) {
                pii curr = { gx, gy };
                while (!(curr.first == sx && curr.second == sy)) {
                    path.push_back(curr);
                    curr = came_from[curr];
                }
                path.push_back({ sx, sy });
                std::reverse(path.begin(), path.end());
                return true;
            }

            for (int i = 0; i < 8; ++i) {
                int nx = x + dxs[i];
                int ny = y + dys[i];
                if (nx < 0 || ny < 0 || nx >= mapSize || ny >= mapSize) continue;
                int grid_value = map[ny][nx];
                if (!is_walkable(grid_value)) continue;

                // diagonal moves cost slightly more
                double move_cost = (dxs[i] == 0 || dys[i] == 0) ? 1.0 : 1.41421356237;
                double tentative_g = g_score[y][x] + move_cost;

                if (tentative_g < g_score[ny][nx]) {
                    came_from[{nx, ny}] = { x, y };
                    g_score[ny][nx] = tentative_g;
                    double fscore = tentative_g + heuristic(nx, ny);
                    open.push({ fscore, { nx, ny } });
                }
            }
        }

        return false; // no path
    }
}

