#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <random>

#include "map.h"

namespace Maze {
    // Directions: up, down, left, right
    std::vector<std::pair<int, int>> directions = {
        {-2, 0}, {2, 0}, {0, -2}, {0, 2}
    };


    // Shuffle directions
    void shuffle_directions() {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(directions.begin(), directions.end(), g);
    }

    // Recursive maze generation using DFS
    void generate_maze(int map[map_size][map_size], int start_x, int start_y) {
        shuffle_directions();

        for (const auto& dir : directions) {
            int nx = start_x + dir.first;
            int ny = start_y + dir.second;

            if (nx > 0 && ny > 0 && nx < map_size - 1 && ny < map_size - 1 && map[nx][ny] == 4) {
                map[nx][ny] = 5;
                map[start_x + dir.first / 2][start_y + dir.second / 2] = 5;
                generate_maze(map, nx, ny);
            }
        }
    }
}

    // // Create maze starting from the coordinate of value 4
    // void create_maze_from_value(int map[map_size][map_size]) {
    //     int start_x = -1, start_y = -1;
    //     bool found = false;

    //     // Locate the value 4 in the map
    //     for (int i = 0; i < map_size && !found; ++i) {
    //         for (int j = 0; j < map_size && !found; ++j) {
    //             if (map[i][j] == 4) {
    //                 start_x = (i % 2 == 0) ? i + 1 : i;
    //                 start_y = (j % 2 == 0) ? j + 1 : j;
    //                 found = true;
    //             }
    //         }
    //     }

    //     if (!found) {
    //         std::cerr << "Value 4 not found in map.\n";
    //         return;
    //     }

    //     // Fill the map with walls
    //     for (int i = 0; i < map_size; ++i)
    //         for (int j = 0; j < map_size; ++j)
    //             map[i][j] = 1;

    //     // Ensure start is inside bounds
    //     if (start_x >= map_size - 1) start_x = map_size - 2;
    //     if (start_y >= map_size - 1) start_y = map_size - 2;

    //     map[start_x][start_y] = 0;

    //     generate_maze(map, start_x, start_y);
    // }


