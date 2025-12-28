#pragma once
#include <vector>
#include <set>
#include <utility>
#include <unordered_set>

/// @brief Enum for things used in the map generation and rendering
///
/// Some things do not have textures associated with them.
enum Map {
    SPRITESHEET,
    PLAYER,
    SPIDER,
    // spider run
    spider_run45_animation,
    spider_run135_animation,
    spider_run225_animation,
    spider_run315_animation,
    spider_run90_animation,
    spider_run180_animation,
    spider_run270_animation,
    spider_run0_animation,
    // spider walk
    spider_walk45_animation,
    spider_walk135_animation,
    spider_walk225_animation,
    spider_walk315_animation,
    spider_walk90_animation,
    spider_walk180_animation,
    spider_walk270_animation,
    spider_walk0_animation,
    // spider idle
    spider_idle45_animation,
    spider_idle135_animation,
    spider_idle225_animation,
    spider_idle315_animation,
    spider_idle90_animation,
    spider_idle180_animation,
    spider_idle270_animation,
    spider_idle0_animation,
    SECTOR_1_PATHWAY,
    SECTOR_2_PATHWAY,
    SECTOR_3_PATHWAY,
    TREE_TRUNK,
    GROUND_CUBE,
    GROUND_CUBE_SPRITE,
    GRASS_COVER_SPRITE,
    GRASS_COVER,
    GRASS_COVER_SHORT,
    GRASS_COVER_TALL,
    TREE,
    MAZE_GROUND_CUBE,
    MAZE_GROUND_SPRITE,
    YELLOW_CUBE,
    ERROR_CUBE,
    BLUE_CUBE,
    WALL_CUBE,
    WALL_CUBE_GROUND,
    WALL_CUBE_SPRITE,
    INGROWN_WALL_CUBE,
    VINE_CUBE_HARD,
    VINE_CUBE_MEDIUM,
    VINE_CUBE_SOFT,
    SECTOR_1_WALL_VAL, // no texture
    SECTOR_2_WALL_VAL, // no texture
    SECTOR_3_WALL_VAL, // no texture
    VINE_OVERHANG_SN,
    VINE_OVERHANG_EW,
    VINE_COVER_N,
    VOID_CUBE,
    VOID_CUBE_NEIGHBOUR,
    INVISIBLE_CUBE,
    WALL_MARKINGS,
    MAZE_DECO,
    MAZE_NS_DOOR,
    MAZE_WE_DOOR,
};

static const std::unordered_set<int> wallValues = {
    Map::WALL_CUBE, Map::INGROWN_WALL_CUBE,
    Map::SECTOR_1_WALL_VAL, Map::SECTOR_2_WALL_VAL, Map::SECTOR_3_WALL_VAL
};

static const std::unordered_set<int> groundValues = {
    Map::GROUND_CUBE, Map::TREE, Map::GRASS_COVER, Map::TREE_TRUNK
};

/* map */
const int mapSize = 325;
extern int map[mapSize][mapSize];

void generate_map();
void print_map(int map[mapSize][mapSize]);
void generate_random_map(int map[mapSize][mapSize], int min_val, int max_val);
void generate_maze_runner_map(int map[mapSize][mapSize]);
void generate_voids(std::set<std::pair<int, int>> void_locations, const int max_voids);