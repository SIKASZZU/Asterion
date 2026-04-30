#pragma once
#include <utility>

extern std::set<std::pair<int, int>> change_gP;
extern std::set<std::pair<int, int>> unchangableWalls_S3;
extern std::vector<std::pair<int, int>> diagonalGrids;
extern std::pair<int, int> gP;
extern int gV;
void mod_map_sector_3();
void seperate_4_sections();