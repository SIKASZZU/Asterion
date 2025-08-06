#include "map.hpp"
#include "render.hpp"
#include "mod_map_data.hpp"
std::set<std::pair<int, int>> change_gP;
std::set<std::pair<int, int>> unchangable_walls_s3;
std::pair<int, int> gP;
int gV;

void mod_map_sector_3() {
    for (int row = 0; row < map_size; row++) {
        for (int column = 0; column < map_size; column++) {
            gP = { row, column };
            gV = map[row][column];
            switch (gV) {
                case Map::SECTOR_3_WALL_VAL: {
                    // Directions: down, right, up, left
                    static const int dr[4] = { +1,  0, -1,  0 };
                    static const int dc[4] = {  0, +1,  0, -1 };

                    int count = 0;
                    for (int i = 0; i < 4; ++i) {
                        int r = row + dr[i];
                        int c = column + dc[i];

                        // (Optional) guard against out-of-bounds if your map edges are not padded:
                        if (r < 0 || r >= map_size || c < 0 || c >= map_size)
                            continue;
                        if (wall_values.find(map[r][c]) != wall_values.end()) {
                            ++count;
                        }
                    }

                    if (count == 4) {
                        unchangable_walls_s3.insert(gP);
                        break;
                    }
                    if (map[row + 1][column] == Map::MAZE_GROUND_CUBE
                        && map[row - 1][column] == Map::MAZE_GROUND_CUBE
                        || map[row][column + 1] == Map::MAZE_GROUND_CUBE
                        && map[row][column - 1] == Map::MAZE_GROUND_CUBE) {
                        unchangable_walls_s3.insert(gP);
                        break;
                    }
                    change_gP.insert(gP);
                    break;
                }
            }
        }
    }
    for ( const auto[x, y] : change_gP) {
        map[x][y] = Map::MAZE_GROUND_CUBE;
    }
}