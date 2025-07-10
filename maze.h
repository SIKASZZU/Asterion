#include <vector>
#include <iostream>

#include "map.h"


namespace Maze{
    extern std::vector<std::pair<int, int>> directions;
    void shuffle_directions();
    void generate_maze(int map[map_size][map_size], int start_x, int start_y);
}


