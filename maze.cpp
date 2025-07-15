#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <random>
#include <queue>
#include <utility>
#include <unordered_map>

#include "map.h"
#include "game.h"

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
    int pathway = 4;

    // Shuffle directions
    void shuffle_directions(std::vector<std::pair<int, int>>& directions) {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(directions.begin(), directions.end(), g);
    }

    // Recursive maze generation using DFS
    void generate_maze(int map[map_size][map_size], int start_x, int start_y, std::string type) {
        std::vector<std::pair<int, int>> directions;
        int allowed_number;

        if (type == "one") {
            directions = directions_sec_1;
            allowed_number = 91;
        }
        else if (type == "two") {
            directions = directions_sec_2;
            allowed_number = 92;
        }
        else if (type == "three") {
            directions = directions_sec_3;
            allowed_number = 93;
        }

        shuffle_directions(directions);

        for (const auto& dir : directions) {
            int nx = start_x + dir.first;
            int ny = start_y + dir.second;

            if (map[nx][ny] != allowed_number) {
                if (map[nx][ny] == pathway && type != "one") {
                    continue;
                }
                continue;
            }

            if (nx > 0 && ny > 0 && nx < map_size - 1 && ny < map_size - 1) {
                map[nx][ny] = pathway;
                map[start_x + (dir.first != 0 ? dir.first / 2 : dir.first)][start_y + (dir.second != 0 ? dir.second / 2 : dir.second)] = pathway;

                if (type == "two" || type == "three") {
                    map[start_x + (dir.first != 0 ? dir.first + 2 : dir.first)][start_y + (dir.second != 0 ? dir.second + 2 : dir.second)] = pathway;
                    map[start_x + (dir.first != 0 ? dir.first + 3 : dir.first)][start_y + (dir.second != 0 ? dir.second + 3 : dir.second)] = pathway;
                    map[start_x + (dir.first != 0 ? dir.first + 1 : dir.first)][start_y + (dir.second != 0 ? dir.second + 1 : dir.second)] = pathway;
                }

                if (type == "three") {
                    map[start_x + (dir.first != 0 ? dir.first - 3 : dir.first)][start_y + (dir.second != 0 ? dir.second - 3 : dir.second)] = pathway;
                    map[start_x + (dir.first != 0 ? dir.first - 1 : dir.first)][start_y + (dir.second != 0 ? dir.second - 1 : dir.second)] = pathway;
                }

                generate_maze(map, nx, ny, type);
            }
        }
    }

    // BFS Pathfinding from (sx, sy) to (gx, gy)
    bool find_path(int map[map_size][map_size], int sx, int sy, int gx, int gy) {
        std::cout << "Starting pathfinder" << "\n";

        std::queue<std::pair<int, int>> q;

        for (const auto& p : path) {
            // undo myself (path)
            std::cout << "(" << p.first << "," << p.second << ") ";
            map[p.first][p.second] = pathway;
        }
        path.clear();  // Erases all the elements. 

        std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> came_from;
        bool visited[map_size][map_size] = { false };

        q.push({ sx, sy });
        visited[sx][sy] = true;

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

                for (const auto& p : path) {
                    std::cout << "(" << p.first << "," << p.second << ") ";
                    map[p.first][p.second] = 7;
                }
                std::cout << "\npath found, lenght: " << path.size() << '\n';
                return true;  // path found
            }

            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i], ny = y + dy[i];
                if (nx >= 0 && ny >= 0 && nx < map_size && ny < map_size &&
                    map[nx][ny] == 4 && !visited[nx][ny]) {
                    visited[nx][ny] = true;
                    came_from[{nx, ny}] = { x, y };
                    q.push({ nx, ny });
                }
            }
        }
        std::cout << "path failed" << "\n";
        return false;  // No path found
    }
}

