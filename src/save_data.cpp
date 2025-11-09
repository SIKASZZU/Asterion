#include "map.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

bool mapLoaded = false;
bool mapSaved = false;

// Save the map to a local file
void save_map_locally(int map[mapSize][mapSize]) {
    std::ofstream out("saved_map.txt");

    if (!out) {
        std::cerr << "Error: Could not open file for writing.\n";
        return;
    }

    for (int y = 0; y < mapSize; ++y) {
        for (int x = 0; x < mapSize; ++x) {
            out << map[y][x];
            if (x < mapSize - 1) out << " ";
        }
        out << "\n";
    }

    out.close();
    mapSaved = true;
    std::cout << "Map saved to 'saved_map.txt'.\n";
}


bool load_map_from_file(int map[mapSize][mapSize], const std::string& filename = "saved_map.txt") {
    std::ifstream in(filename);

    if (!in) {
        std::cerr << "Error: Could not open file for loading: " << filename << "\n";
        mapLoaded = false;
        return false;
    }

    std::string line;
    int row = 0;

    while (std::getline(in, line) && row < mapSize) {
        std::istringstream ss(line);
        int col = 0;
        int value;

        while (ss >> value && col < mapSize) {
            map[row][col] = value;
            col++;
        }

        // If a row has fewer/more values than mapSize, you might want to handle it
        if (col != mapSize) {
            std::cerr << "Warning: Row " << row << " has " << col << " values (expected " << mapSize << ")\n";
        }

        row++;
    }

    if (row != mapSize) {
        std::cerr << "Warning: Only " << row << " rows loaded (expected " << mapSize << ")\n";
        mapLoaded = false;
        return false;
    }

    std::cout << "Map loaded from '" << filename << "'.\n";
    mapLoaded = true;
    return true;
}