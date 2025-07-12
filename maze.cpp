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
    std::vector<std::pair<int, int>> directions_norm = {
        {-4, 0}, {4, 0}, {0, -4}, {0, 4}  // normal {-2, 0}, {2, 0}, {0, -2}, {0, 2}
    };

    std::vector<std::pair<int, int>> directions_dot = {
        {-4, 0}, {2, 0}, {0, -4}, {0, 2}  // dots
    };

    std::vector<std::pair<int, int>> path;
   
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
        int change_to;
        if (type == "dot") {
            directions = directions_dot;
            allowed_number = 6;
            change_to      = 66;
        } else {
            directions = directions_norm;
            allowed_number = 4;
            change_to      = 5;
        }
        
        shuffle_directions(directions);

        for (const auto& dir : directions) {
            int nx = start_x + dir.first;
            int ny = start_y + dir.second;

            if (nx > 0 && ny > 0 && nx < map_size - 1 && ny < map_size - 1 && map[nx][ny] == allowed_number) {
                map[nx][ny] = change_to;
                map[start_x + (dir.first != 0 ? dir.first / 2 : dir.first)][start_y + (dir.second != 0 ? dir.second / 2 : dir.second)] = change_to;
                map[start_x + (dir.first != 0 ? dir.first + 2 : dir.first)][start_y + (dir.second != 0 ? dir.second + 2 : dir.second)] = change_to;
                map[start_x + (dir.first != 0 ? dir.first + 3 : dir.first)][start_y + (dir.second != 0 ? dir.second + 3 : dir.second)] = change_to;
                map[start_x + (dir.first != 0 ? dir.first + 1 : dir.first)][start_y + (dir.second != 0 ? dir.second + 1 : dir.second)] = change_to;
                
                if (allowed_number != 6) {
                    map[start_x + (dir.first != 0 ? dir.first - 3 : dir.first)][start_y + (dir.second != 0 ? dir.second - 3 : dir.second)] = change_to;
                    map[start_x + (dir.first != 0 ? dir.first - 1 : dir.first)][start_y + (dir.second != 0 ? dir.second - 1 : dir.second)] = change_to;

                }
                generate_maze(map, nx, ny, type);
            }
        }
    }

    // BFS Pathfinding from (sx, sy) to (gx, gy)
    bool find_path(int map[map_size][map_size], int sx, int sy, int gx, int gy) {
        if (map[sx][sy] != 5 || map[gx][gy] != 5) {
            std::cout << "Invalid start, goal grids" << "\n";
            return false;  // Invalid start/goal
        }
        
        std::cout << "Starting pathfinder" << "\n";

        std::queue<std::pair<int, int>> q;
        
        for (const auto& p : path) {
            // undo myself (path)
            std::cout << "(" << p.first << "," << p.second << ") ";
            map[p.first][p.second] = 5;
        }
        path.clear();  // Erases all the elements. 
        
        std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> came_from;
        bool visited[map_size][map_size] = {false};

        q.push({sx, sy});
        visited[sx][sy] = true;

        int dx[4] = {-1, 1, 0, 0};
        int dy[4] = {0, 0, -1, 1};

        while (!q.empty()) {
            auto [x, y] = q.front(); q.pop();

            // end found
            if (x == gx && y == gy) { 
                // Reconstruct path
                std::pair<int, int> curr = {gx, gy};
                while (curr != std::make_pair(sx, sy)) {
                    path.push_back(curr);
                    curr = came_from[curr];
                }
                path.push_back({sx, sy});
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
                    map[nx][ny] == 5 && !visited[nx][ny]) {
                    visited[nx][ny] = true;
                    came_from[{nx, ny}] = {x, y};
                    q.push({nx, ny});
                }
            }
        }
        std::cout << "path failed" << "\n";
        return false;  // No path found
    }
}

