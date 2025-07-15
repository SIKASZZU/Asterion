#pragma once
#include <vector>
#include <iostream>

#include "map.h"

namespace Maze {
    extern std::vector<std::pair<int, int>> directions_sec_3;
    extern std::vector<std::pair<int, int>> directions_sec_2;
    extern std::vector<std::pair<int, int>> directions_sec_1;

    extern std::vector<std::pair<int, int>> path;
    extern int pathway;

    void shuffle_directions(std::vector<std::pair<int, int>>& directions);
    void generate_maze(int map[map_size][map_size], int start_x, int start_y, std::string type);
    bool find_path(int map[map_size][map_size], int sx, int sy, int gx, int gy);
}


