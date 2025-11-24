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
    // BFS Pathfinding from (sx, sy) to (gx, gy)
    bool find_path(const int map[mapSize][mapSize], int sx, int sy, int gx, int gy) {

        std::queue<std::pair<int, int>> q;
        path.clear();

        std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> came_from;
        bool visited[mapSize][mapSize] = { false };

        q.push({ sx, sy });
        // visited is [row][col] -> [y][x]
        visited[sy][sx] = true;

        int dx[4] = { -1, 1, 0, 0 };
        int dy[4] = { 0, 0, -1, 1 };
        while (!q.empty()) {
            auto [x, y] = q.front(); q.pop();

            // end found
            if (x == gx && y == gy) {
                // Reconstruct path
                std::pair<int, int> curr = { gx, gy };
                while (curr != std::make_pair(sx, sy)) {
                    path.push_back(curr);
                    curr = came_from[curr];
                }
                path.push_back({ sx, sy });
                std::reverse(path.begin(), path.end());
                return true;  // path found
            }

            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i], ny = y + dy[i];
                // map is map[row][col] -> map[y][x]
                if (nx >= 0 && ny >= 0 && nx < mapSize && ny < mapSize) {
                    int grid_value = map[ny][nx];
                    if (is_walkable(grid_value) && !visited[ny][nx]) {
                        visited[ny][nx] = true;
                        came_from[{nx, ny}] = { x, y };
                        q.push({ nx, ny });
                    }
                }
            }
        }
        return false;  // No path found
    }
}

