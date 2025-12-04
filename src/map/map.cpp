#include "map.hpp"
#include "game.hpp"
#include "maze.hpp"
#include "mod_map_data.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <utility>
#include <set>
#include <random>

int map[mapSize][mapSize];  // tra mdea, aga see peab siin uuesti olema ilma externita.

void print_map(int map[mapSize][mapSize]) {
    for (int y = 0; y < mapSize; y++) {
        for (int x = 0; x < mapSize; x++) {
            std::cout << map[y][x] << ' ';
        }
        std::cout << '\n';
    }std::cout << std::endl;
}


void generate_map() {
    /* try to load from file */
    // load_map_from_file(map);  // return boolean mapLoaded

    /* Vali ise, mis mappi tahad geneda. */
    // generate_random_map(map, 2, 2);          // random ring, circular map
    // if (mapLoaded) { return; }

    generate_maze_runner_map(map);              // maze runner based map. WorkInProgress

    bool sector_1 = false, sector_2 = false, sector_3 = false;

    auto find_start_sectors = [&]() -> bool {
        for (int i = 0; i < mapSize; i++) {
            for (int j = 0; j < mapSize; j++) {
                if (map[i][j] == Map::SECTOR_1_WALL_VAL && sector_1 == false) {
                    std::cout << "Generating SECTION 1: Start grid: " << i << " " << j << "\n";
                    Maze::generate_maze(map, i, j, "one");
                    sector_1 = true;
                }
                if (map[i][j] == Map::SECTOR_2_WALL_VAL && sector_2 == false) {
                    std::cout << "Generating SECTION 2: Start grid: " << i << " " << j << "\n";
                    Maze::generate_maze(map, i, j, "two");
                    sector_2 = true;
                }
                if (map[i][j] == Map::SECTOR_3_WALL_VAL && sector_3 == false) {
                    std::cout << "Generating SECTION 3: Start grid: " << i << " " << j << "\n";
                    Maze::generate_maze(map, i, j, "three");
                    sector_3 = true;
                }
                if (sector_1 == true && sector_2 == true && sector_3 == true) return true;
            }
        }
        return false;
        };

    bool sector_status = find_start_sectors();

    mod_map_sector_3();
    seperate_4_sections();
    // print_map(map);
    // save_map_locally(map);
}

// this is not used anywhere and most likely out of date with recent changes
void generate_random_map(int map[mapSize][mapSize], int minVal, int maxVal) {
    float centerX = mapSize / 2.0f;
    float centerY = mapSize / 2.0f;
    float maxDistance = std::sqrt(centerX * centerX + centerY * centerY);

    for (int y = 0; y < mapSize; y++) {
        for (int x = 0; x < mapSize; x++) {
            float dx = y - centerX;
            float dy = x - centerY;
            float distance = std::sqrt(dx * dx + dy * dy) / maxDistance;

            // Inverse chance of land (closer to center = more land)
            float landChance = 1.0f - distance;

            // Add stronger noise (-0.4 to +0.4)
            landChance += ((rand() % 100) / 100.0f - 0.5f) * 0.8f;

            landChance = 1.0f ? landChance > 1.0f : landChance = 0.0f;  // Clamp to 0–1

            if (landChance > 0.4f) {
                // Land: elevation between mid and max
                int landVal = minVal + (maxVal - minVal) / 2 + rand() % ((maxVal - minVal) / 2 + 1);
                map[y][x] = landVal;
            }
            else {
                // Water
                map[y][x] = minVal;
            }
        }
    }
}


void generate_maze_runner_map(int map[mapSize][mapSize]) {
    int halfMapSize = mapSize / 2;
    float maxDistance = std::sqrt(halfMapSize * halfMapSize + halfMapSize * halfMapSize);

    /* Glade */
    int gladeRadius = (mapSize / 10) >= 10 ? 10 : mapSize / 10;  // if mapSize / 10 > 10; hard cap to 10.

    int mazeInnerRadius = gladeRadius + 1;
    signed int mazeSize = mapSize / 2 - 4;
    signed int maxMazeSize = 130;
    int mazeOuterRadius = std::min(mazeSize, maxMazeSize);
    std::cout << "Maze size (var mazeOuterRadius) is: " << mazeOuterRadius << '\n';
    std::cout << "Map size is: " << mapSize << '\n';
    int mazeThirdSector = mazeOuterRadius - (mazeOuterRadius / 3);
    int mazeSecondSector = mazeOuterRadius - (mazeOuterRadius / 2);

    // void spawnpoints, chooses randomly from set int maxVoids
    std::set<std::pair<int, int>> voidSpawnpoints;

    const int numSectors = 8;
    const int maxVoids = 10;

    for (int y = 0; y < mapSize; y++) {
        for (int x = 0; x < mapSize; x++) {

            int dx = static_cast<int>(std::abs(x - halfMapSize));
            int dy = static_cast<int>(std::abs(y - halfMapSize));

            float distance_sq = dx * dx + dy * dy;
            float distance = std::sqrt(distance_sq);

            float angle = std::atan2(dy, dx);
            if (angle < 0) angle += 2 * PI;
            float sector_angle = 2 * PI / numSectors;

            float norm_dist = distance / maxDistance;
            float landChance = 1.0f - norm_dist;
            landChance += ((rand() % 100) / 100.0f - 0.5f) * 0.8f;
            landChance = std::clamp(landChance, 0.0f, 1.0f);

            if (distance >= mazeOuterRadius) {
                if (landChance != 0.0f) {
                    map[y][x] = Map::GROUND_CUBE;
                }
                else {
                    rand() % 2 == 1 ? map[y][x] = Map::TREE_TRUNK : map[y][x] = Map::TREE;
                }
            }

            // maze ala full maze_ground, section overwritib oma enda dataga.
            if (distance >= mazeInnerRadius
                && distance <= mazeOuterRadius) {
                map[y][x] = Map::MAZE_GROUND_CUBE;
            }
            int drawbackBuffer = 3;
            if (distance >= mazeInnerRadius
                && distance <= mazeSecondSector - drawbackBuffer) {
                map[y][x] = Map::SECTOR_1_WALL_VAL;
            }
            else if (distance >= mazeSecondSector && distance <= mazeThirdSector) {
                map[y][x] = Map::SECTOR_2_WALL_VAL;
            }
            else if (distance >= mazeThirdSector && distance <= mazeOuterRadius) {
                map[y][x] = Map::SECTOR_3_WALL_VAL;
            }

            for (int sector = 0; sector < numSectors; ++sector) {
                float wallAngle = sector * sector_angle;
                // normalize to [-PI, PI]
                float delta = std::fmod(angle - wallAngle + PI, 2 * PI) - PI;
                if (distance >= mazeOuterRadius) {
                    float sizeOfForest = 0.3f;
                    if (sector % 2 != 0
                        && std::fabs(delta) < sizeOfForest
                        && landChance <= 0.3f) {
                        rand() % 125 == 1 ? map[y][x] = Map::TREE_TRUNK : map[y][x] = Map::TREE;
                    }
                    // mapi nurkades on suured metsad o_o, 
                    // metsades on portaalid O_O, 
                    // varsti saab neist sisse minna (❁´◡`❁)
                    if (sector % 2 != 0
                        && std::fabs(delta) < 0.15f
                        && distance <= halfMapSize + 25
                        && distance >= mazeOuterRadius + 40) {
                        // todo: fix the +25 ja +40
                        voidSpawnpoints.insert({ y, x });
                    }
                }
                // diagonaalidel mazei vahekohad
                if (sector % 2 != 0) {
                    if (distance <= (mazeSecondSector * 0.7)
                        && (distance >= mazeInnerRadius * 3.3)) {
                        if (std::fabs(delta) < 0.5) {  // kontrollib section wallide thicknessi.
                            map[y][x] = Map::MAZE_GROUND_CUBE;
                        }
                    }
                }
                // pathwayd suunas kell 12, 3, 6, 9, et player gladeist minema saaks.
                else if (sector % 2 == 0 && distance <= (mazeSecondSector / 2.5)) {
                    if (std::fabs(delta) < 0.08) {  // kontrollib section wallide thicknessi.
                        map[y][x] = Map::MAZE_GROUND_CUBE;
                    }
                }
                // walls between sector 1/2 
                int thicknessSectionWall = 2;
                if (distance <= mazeSecondSector
                    && distance >= mazeSecondSector - thicknessSectionWall
                    && std::fabs(delta) < 0.3) {
                    map[y][x] = Map::INGROWN_WALL_CUBE;
                }
            }
            // walls between sector 3/outside
            if (distance >= mazeOuterRadius
                && distance <= mazeOuterRadius + 2) {
                if (distance >= mazeOuterRadius + 1
                    && distance <= mazeOuterRadius + 2) {
                    map[y][x] = Map::MAZE_GROUND_CUBE;
                }
                else {
                    map[y][x] = Map::INGROWN_WALL_CUBE;
                }
            }
            // walls between sector 2/3 
            if (distance <= mazeThirdSector
                && distance >= mazeThirdSector - 1) {
                map[y][x] = Map::INGROWN_WALL_CUBE;
            }

            // walls around the map, map border.
            if (y == 0 || x == 0 || y == (mapSize - 1) || x == (mapSize - 1)) {
                map[y][x] = Map::INGROWN_WALL_CUBE;
            }
            // Glade (square) and Ingrown walls around glade
            int thicknessGladeWall = 1;
            if (dx <= gladeRadius + thicknessGladeWall
                && dy <= gladeRadius + thicknessGladeWall) {
                // walls outside of glade radius
                if (dx == gladeRadius + 1
                    || dy == gladeRadius + 1) {
                    if (x != halfMapSize && y != halfMapSize) {
                        map[y][x] = Map::INGROWN_WALL_CUBE;
                    }
                    if (dx == 0) {
                        map[y][x] = Map::MAZE_NS_DOOR;
                    }
                    if (dy == 0) {
                        map[y][x] = Map::MAZE_WE_DOOR;
                    }
                }
                // inner glade
                else {
                    map[y][x] = Map::GROUND_CUBE;
                }
            }
            if (distance <= mazeOuterRadius
                && dx > gladeRadius + 1
                && dy > gladeRadius + 1) {
                if (y == x || x == mapSize - y - 1) diagonalGrids.push_back(std::make_pair(y, x));
            }
        }
    }
    std::cout << "Possible void spawnpoints in set: " << voidSpawnpoints.size() << '\n';
    generate_voids(voidSpawnpoints, maxVoids);
}


void generate_voids(std::set<std::pair<int, int>> voidLocations, const int maxVoids) {
    if (voidLocations.empty()) std::cout << "Alert: No voids spawned! voidLocations.empty()";

    std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<size_t> dist(0, voidLocations.size() - 1);

    std::vector<std::pair<int, int>> locations(voidLocations.begin(), voidLocations.end());

    if (locations.empty()) { std::cout << "Error turning set into vector in generate_voids. Returned."; return; }

    for (int i = 0; i < maxVoids; i++) {
        auto [y, x] = locations[dist(rng)];
        std::cout << "Assigned void pair to map data: " << y << " " << x << '\n';
        map[y][x] = Map::VOID_CUBE;
    }
}