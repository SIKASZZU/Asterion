#pragma once
#include <vector>
#include <set>
#include <utility>

/// @brief Enum for things used in the map generation and rendering
///
/// Some things do not have textures associated with them.
enum Map {
    PLAYER,
    LAND, // no texture
    TREE,
    TREE_TRUNK,
    GROUND_CUBE,
    MAZE_GROUND_CUBE,
    YELLOW_CUBE,
    ERROR_CUBE,
    BLUE_CUBE,
    WALL_CUBE,
    INGROWN_WALL_CUBE,
    VINE_WALL, // no texture
    VINE_CUBE_HARD,
    VINE_CUBE_MEDIUM,
    VINE_CUBE_SOFT,
    SECTOR_1_WALL_VAL, // no texture
    SECTOR_2_WALL_VAL, // no texture
    SECTOR_3_WALL_VAL, // no texture
    NUMBER_ATLAS,
    VINE_OVERHANG_SN,
    VINE_OVERHANG_EW,
    VINE_COVER_N,
    VOID_CUBE,
    VOID_CUBE_NEIGHBOUR,
    GRASS_COVER,
    INVISIBLE_CUBE,
};


/* map */
const int map_size = 325;
extern int map[map_size][map_size];
extern std::vector<std::pair<int, int>> maze_door_grids;

void generate_map();
void print_map(int map[map_size][map_size]);
void generate_random_map(int map[map_size][map_size], int min_val, int max_val);
void generate_maze_runner_map(int map[map_size][map_size]);
void generate_voids(std::set<std::pair<int,int>> void_locations, const int max_voids);