#pragma once
#include <vector>


// TODO: this can be an enum
/* map values */
namespace Map {
    const int MIN_VAL = 0;
    const int LAND_VAL = 1;
    const int TREE_VAL = 2;
    const int GROUND_CUBE = 3;
    const int MAZE_GROUND_VAL = 4;
    const int SNOWY_GROUND_VAL = 5;
    const int YELLOW_VAL = 6;
    const int BLUE_VAL = 66;
    const int ERROR_VAL = 7;
    const int WALL_VAL_VINE = 8;
    const int WALL_VAL = 9;
    const int NUMBER_ATLAS = 10;

    const int SECTOR_1_WALL_VAL = 91;
    const int SECTOR_2_WALL_VAL = 92;
    const int SECTOR_3_WALL_VAL = 93;

    const int INGROWN_WALL_VAL = 94;

    const int CUBE_VINE_HARD_TEX = 100;
    const int CUBE_VINE_MEDIUM_TEX = 101;
    const int CUBE_VINE_SOFT_TEX = 102;


}


/* map */
const int map_size = 250;
extern int map[map_size][map_size];
extern std::vector<std::pair<int, int>> maze_door_grids;

void generate_map();
void print_map(int map[map_size][map_size]);
void generate_random_map(int map[map_size][map_size], int min_val, int max_val);
void generate_maze_runner_map(int map[map_size][map_size]);
