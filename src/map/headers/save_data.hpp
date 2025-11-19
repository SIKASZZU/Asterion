#pragma once
#include "map.hpp"
#include <string>


extern bool mapLoaded;
extern bool mapSaved;

bool load_map_from_file(int map[mapSize][mapSize], const std::string& filename = "saved_map.txt");
void save_map_locally(int map[mapSize][mapSize]);