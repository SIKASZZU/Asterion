#pragma once
#include "map.hpp"
#include <string>


extern bool map_loaded;
extern bool map_saved;

bool load_map_from_file(int map[map_size][map_size], const std::string& filename = "saved_map.txt");
void save_map_locally(int map[map_size][map_size]);