#include "map.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool map_loaded = false; 
bool map_saved = false;

// Save the map to a local file
void save_map_locally(int map[map_size][map_size]) {
    std::ofstream out("saved_map.txt");

    if (!out) {
        std::cerr << "Error: Could not open file for writing.\n";
        return;
    }

    for (int y = 0; y < map_size; ++y) {
        for (int x = 0; x < map_size; ++x) {
            out << map[y][x];
            if (x < map_size - 1) out << " ";
        }
        out << "\n";
    }

    out.close();
    map_saved = true;
    std::cout << "Map saved to 'saved_map.txt'.\n";
}


bool load_map_from_file(int map[map_size][map_size], const std::string& filename = "saved_map.txt") {
    std::ifstream in(filename);

    if (!in) {
        std::cerr << "Error: Could not open file for loading: " << filename << "\n";
        map_loaded = false;
        return false;
    }

    std::string line;
    int row = 0;

    while (std::getline(in, line) && row < map_size) {
        std::istringstream ss(line);
        int col = 0;
        int value;

        while (ss >> value && col < map_size) {
            map[row][col] = value;
            col++;
        }

        // If a row has fewer/more values than map_size, you might want to handle it
        if (col != map_size) {
            std::cerr << "Warning: Row " << row << " has " << col << " values (expected " << map_size << ")\n";
        }

        row++;
    }

    if (row != map_size) {
        std::cerr << "Warning: Only " << row << " rows loaded (expected " << map_size << ")\n";
        map_loaded = false;
        return false;
    }

    std::cout << "Map loaded from '" << filename << "'.\n";
    map_loaded = true;
    return true;
}