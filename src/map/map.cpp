#include "map.hpp"
#include "game.hpp"
#include "maze.hpp"
#include "mod_map_data.hpp"
#include "render.hpp"

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
    std::cout << "Map generation starting...\n";
    /* try to load from file */
    // load_map_from_file(map);  // return boolean mapLoaded

    /* Vali ise, mis mappi tahad geneda. */
    // generate_random_map(map, 2, 2);          // random ring, circular map
    // if (mapLoaded) { return; }

    bool test = true;

    if (test) {
        generate_test_map(map);
        return;
    }
    else {
        generate_maze_runner_map(map);              // maze runner based map. WorkInProgress
    }

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
    generate_decorations();
    // print_map(map);
    // save_map_locally(map);
    std::cout << "Map generation done.\n";
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
                map[y][x] = Map::SECTOR_1_PATHWAY;
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
                            map[y][x] = Map::SECTOR_1_PATHWAY;
                        }
                    }
                }
                // pathwayd suunas kell 12, 3, 6, 9, et player gladeist minema saaks.
                else if (sector % 2 == 0 && distance <= (mazeSecondSector / 2.5)) {
                    if (std::fabs(delta) < 0.08) {  // kontrollib section wallide thicknessi.
                        map[y][x] = Map::SECTOR_1_PATHWAY;
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
                    map[y][x] = Map::SECTOR_1_PATHWAY;
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

void generate_test_map(int map[mapSize][mapSize]) {

    int testIslandRadius = static_cast<int>(mapSize * 0.7);
    int halfMapSize = mapSize / 2;
    float maxDistance = std::sqrt(halfMapSize * halfMapSize + halfMapSize * halfMapSize);


    for (int y = 0; y < mapSize; y++) {
        for (int x = 0; x < mapSize; x++) {

            int dx = static_cast<int>(std::abs(x - halfMapSize));
            int dy = static_cast<int>(std::abs(y - halfMapSize));

            float distance_sq = dx * dx + dy * dy;
            float distance = std::sqrt(distance_sq);

            if (distance <= testIslandRadius) {
                map[y][x] = Map::GROUND_CUBE;
                continue;
            }
            if (distance > testIslandRadius) {
                map[y][x] = Map::ERROR_CUBE;
                continue;
            }
        }
    }

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

void generate_decorations() {
    // Precompute decoration indices so renderer can just render.

    decorationIndexMap.clear();

    for (int row = 0; row < mapSize; row++) {
        for (int column = 0; column < mapSize; column++) {

            uint32_t key = make_grid_key(row, column);
            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };

            switch (gridValue) {
            case Map::VINE_COVER_N:
            case Map::VINE_OVERHANG_SN:
            case Map::VINE_OVERHANG_EW: {
                break;
            }
                                      // case Map::MAZE_GROUND_CUBE: {
            case Map::SECTOR_3_WALL_VAL:
            case Map::SECTOR_2_PATHWAY:
            case Map::SECTOR_3_PATHWAY: {
                if (rand() % mazeDecoMod == 0) {
                    // save to spritesheetIndexMap
                    int idx = ensure_spritesheet_index_for_row(gridPos, ssi::coverMaze);
                    decorationIndexMap.try_emplace(key, idx);
                }
                break;
            }
            case Map::GROUND_CUBE: {
                if (rand() % groundDecoMod == 0) {
                    // save to spritesheetIndexMap
                    int idx = ensure_spritesheet_index_for_row(gridPos, ssi::coverGround);
                    decorationIndexMap.try_emplace(key, idx);
                }
                break;
            }
            case Map::SECTOR_2_WALL_VAL: {

                // if (gridValue == Map::SECTOR_2_WALL_VAL) { srcFRect = { 32, 0, 32, 32 }; }
                // else { srcFRect = { 0, 0, 32, 32 }; };
                // // create wall markings nr2 (100 divided by % number, 20% chance)
                // int varIndex = vineDecorationIndexMap.try_emplace(make_grid_key(row, column), rand() % 5).first->second;
                // if (varIndex == 4) {
                //     renderQueue.push_back(
                //         RenderQueueItem(destTile.y + 1, srcFRect, destTile, &textureMap[Map::WALL_MARKINGS], alpha)
                //     );
                // }

                // if (gridValue != Map::SECTOR_2_WALL_VAL) break;
                // // vaata grid_below, sest Vine tekstuur on vaid NS orientatsiooniga
                // // create hanging vines from wall to wall
                // int grid_below = map[row - 1][column];
                // if ((rand() % vinesDecoMod == 0) && !gridVineChecked.count(gridPos)) {
                //     // add overhang vines VINE_OVERHANG_SN
                //     if (grid_below == Map::MAZE_GROUND_CUBE &&
                //         (map[row - 2][column] == Map::SECTOR_2_WALL_VAL)) {
                //         map[row - 1][column] = Map::VINE_OVERHANG_SN;

                //         int idx = ensure_spritesheet_index_for_row(gridPos, ssi::coverMaze);
                //         decorationIndexMap.try_emplace(key, idx);
                //     }
                //     // add overhang vines VINE_OVERHANG_EW
                //     if (map[row][column + 1] == Map::MAZE_GROUND_CUBE &&
                //         (map[row][column + 2] == Map::SECTOR_2_WALL_VAL)) {
                //         map[row][column + 1] = Map::VINE_OVERHANG_EW;
                //     }
                // }

                // // create vines on the sides of walls
                // if (rand() % 10 == 1 && !gridVineChecked.count(gridPos)) {
                //     // add VINE_COVER_N to wall's southern side
                //     if (grid_below == Map::MAZE_GROUND_CUBE ||
                //         grid_below == Map::VINE_OVERHANG_SN ||
                //         grid_below == Map::VINE_OVERHANG_EW) {
                //         map[row - 1][column] = Map::VINE_COVER_N;
                //         // expand vine to neighbouring blocks aswell.
                //         // FIXME: siin on bug. Kui vine on 6hus ss see ie ole feature. hetkel mitte.
                //         // Should be recursion but maze direction doesn't allow longer than 3 walls in sector2
                //         if (map[row][column + 1] == Map::VINE_OVERHANG_SN ||
                //             map[row][column + 1] == Map::VINE_OVERHANG_EW) {
                //             map[row - 1][column + 1] = Map::VINE_COVER_N;
                //         }
                //         if (map[row][column - 1] == Map::VINE_OVERHANG_SN ||
                //             map[row][column - 1] == Map::VINE_OVERHANG_EW) {
                //             map[row - 1][column - 1] = Map::VINE_COVER_N;
                //         }
                //     }
                // }
                // gridVineChecked.insert(gridPos);
                // auto cubeVineTex = choose_cube_vine_texture("", gridPos);
                // if (cubeVineTex != nullptr) {
                //     renderQueue.push_back(
                //         RenderQueueItem(destTile.y + halfTile + 2, destTile, cubeVineTex, alpha)
                //     );
                // }
                break;
            }
                                       // case Map::SECTOR_3_WALL_VAL: {
                                       //     // create wall markings

                                       //     // if (!isEmpty(srcFRect)) renderQueue.push_back(RenderQueueItem(destTile.y + 1, srcFRect, destTile, &textureMap[Map::WALL_MARKINGS], alpha));

                                       //     // 3rd sector decorations on top of walls
                                       //     // if (rand() % 3 != 1) break;
                                       //     break;
                                       // }
            }
        }
    }
}

