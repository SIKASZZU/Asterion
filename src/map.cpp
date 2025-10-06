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

int map[map_size][map_size];  // tra mdea, aga see peab siin uuesti olema ilma externita.

void print_map(int map[map_size][map_size]) {
    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            std::cout << map[y][x] << ' ';
        }
        std::cout << '\n';
    }std::cout << std::endl;
}


void generate_map() {
    /* try to load from file */
    // load_map_from_file(map);  // return boolean map_loaded

    /* Vali ise, mis mappi tahad geneda. */
    // generate_random_map(map, 2, 2);          // random ring, circular map
    // if (map_loaded) { return; }

    generate_maze_runner_map(map);              // maze runner based map. WorkInProgress

    bool sector_1 = false, sector_2 = false, sector_3 = false;

    auto find_start_sectors = [&]() -> bool {
        for (int i = 0; i < map_size; i++) {
            for (int j = 0; j < map_size; j++) {
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
    // print_map(map);
    // save_map_locally(map);
}

// this is not used anywhere and most likely out of date with recent changes
void generate_random_map(int map[map_size][map_size], int min_val, int max_val) {
    float center_x = map_size / 2.0f;
    float center_y = map_size / 2.0f;
    float max_distance = std::sqrt(center_x * center_x + center_y * center_y);

    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            float dx = y - center_x;
            float dy = x - center_y;
            float distance = std::sqrt(dx * dx + dy * dy) / max_distance;

            // Inverse chance of land (closer to center = more land)
            float land_chance = 1.0f - distance;

            // Add stronger noise (-0.4 to +0.4)
            land_chance += ((rand() % 100) / 100.0f - 0.5f) * 0.8f;

            land_chance = 1.0f ? land_chance > 1.0f : land_chance = 0.0f;  // Clamp to 0–1

            if (land_chance > 0.4f) {
                // Land: elevation between mid and max
                int land_val = min_val + (max_val - min_val) / 2 + rand() % ((max_val - min_val) / 2 + 1);
                map[y][x] = land_val;
            }
            else {
                // Water
                map[y][x] = min_val;
            }
        }
    }
}


void generate_maze_runner_map(int map[map_size][map_size]) {
    int half_map_size = map_size / 2;
    float max_distance = std::sqrt(half_map_size * half_map_size + half_map_size * half_map_size);

    /* Glade */
    int glade_radius = (map_size / 10) >= 10 ? 10 : map_size / 10;  // if map_size / 10 > 10; hard cap to 10.

    int maze_inner_radius = glade_radius + 1;
    signed int maze_size = map_size / 2 - 4;
    signed int max_maze_size = 130;
    int maze_outer_radius = std::min(maze_size, max_maze_size);
    std::cout << "Maze size (var maze_outer_radius) is: " << maze_outer_radius << '\n';
    std::cout << "Map size is: " << map_size << '\n';
    int maze_third_sector = maze_outer_radius - (maze_outer_radius / 3);
    int maze_second_sector = maze_outer_radius - (maze_outer_radius / 2);

    // void spawnpoints, chooses randomly from set int max_voids
    std::set<std::pair<int, int>> void_spawnpoints;

    const int num_sectors = 8;
    const int max_voids = 10;

    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {

            float dx = x - half_map_size;
            float dy = y - half_map_size;

            float distance_sq = dx * dx + dy * dy;
            float distance = std::sqrt(distance_sq);

            float angle = std::atan2(dy, dx);
            if (angle < 0) angle += 2 * PI;
            float sector_angle = 2 * PI / num_sectors;

            float norm_dist = distance / max_distance;
            float land_chance = 1.0f - norm_dist;
            land_chance += ((rand() % 100) / 100.0f - 0.5f) * 0.8f;
            land_chance = std::clamp(land_chance, 0.0f, 1.0f);
            
            if (distance >= maze_outer_radius) {
                if (land_chance != 0.0f) {
                    map[y][x] = Map::GROUND_CUBE;
                } else {
                    rand() % 2 == 1 ? map[y][x] = Map::TREE_TRUNK : map[y][x] = Map::TREE;
                }
            }

            // maze ala full maze_ground, section overwritib oma enda dataga.
            if (distance >= maze_inner_radius 
                && distance <= maze_outer_radius) {
                map[y][x] = Map::MAZE_GROUND_CUBE;
            }
            int drawback_buffer = 3;
            if (distance >= maze_inner_radius 
                && distance <= maze_second_sector - drawback_buffer) {
                map[y][x] = Map::SECTOR_1_WALL_VAL;
            }
            else if (distance >= maze_second_sector && distance <= maze_third_sector) {
                map[y][x] = Map::SECTOR_2_WALL_VAL;
            }
            else if (distance >= maze_third_sector && distance <= maze_outer_radius) {
                map[y][x] = Map::SECTOR_3_WALL_VAL;
            }
            
            for (int sector = 0; sector < num_sectors; ++sector) {
                float wall_angle = sector * sector_angle;
                // normalize to [-PI, PI]
                float delta = std::fmod(angle - wall_angle + PI, 2 * PI) - PI;
                if (distance >= maze_outer_radius) {
                    float size_of_forest = 0.3f;
                    if (sector % 2 != 0
                        && std::fabs(delta) < size_of_forest
                        && land_chance <= 0.3f) {
                        rand() % 125 == 1 ? map[y][x] = Map::TREE_TRUNK : map[y][x] = Map::TREE;
                    }
                    // mapi nurkades on suured metsad o_o, 
                    // metsades on portaalid O_O, 
                    // varsti saab neist sisse minna (❁´◡`❁)
                    if (sector % 2 != 0
                        && std::fabs(delta) < 0.15f
                        && distance <= half_map_size + 25
                        && distance >= maze_outer_radius + 40) {
                        // todo: fix the +25 ja +40
                        void_spawnpoints.insert({ y, x });
                    }
                }
                // diagonaalidel mazei vahekohad
                if (sector % 2 != 0 
                    && distance <= (maze_second_sector * 0.7) 
                    && (distance >= maze_inner_radius * 3.3)) {
                    if (std::fabs(delta) < 0.5) {  // kontrollib section wallide thicknessi.
                        map[y][x] = Map::MAZE_GROUND_CUBE;
                    }
                }
                // pathwayd suunas kell 12, 3, 6, 9, et player gladeist minema saaks.
                else if (sector % 2 == 0 && distance <= (maze_second_sector / 2.5)) {
                    if (std::fabs(delta) < 0.08) {  // kontrollib section wallide thicknessi.
                        map[y][x] = Map::MAZE_GROUND_CUBE;
                    }
                }
                // walls between sector 1/2 
                int thickness_section_wall = 2;
                if (distance <= maze_second_sector
                    && distance >= maze_second_sector - thickness_section_wall
                    && std::fabs(delta) < 0.3) { 
                    map[y][x] = Map::INGROWN_WALL_CUBE; 
                }
            }
            // walls between sector 3/outside
            if (distance >= maze_outer_radius 
                && distance <= maze_outer_radius + 2) {
                if (distance >= maze_outer_radius + 1 
                    && distance <= maze_outer_radius + 2) {
                    map[y][x] = Map::MAZE_GROUND_CUBE;
                }
                else {
                    map[y][x] = Map::INGROWN_WALL_CUBE;
                }
            }
            // walls between sector 2/3 
            if (distance <= maze_third_sector
                && distance >= maze_third_sector - 1) { 
                map[y][x] = Map::INGROWN_WALL_CUBE; 
            }

            // walls around the map, map border.
            if (y == 0 || x == 0 || y == (map_size - 1) || x == (map_size - 1)) {
                map[y][x] = Map::INGROWN_WALL_CUBE;
            }
            // Glade (square) and Ingrown walls around glade
            int glade_wall_thickness = 1;
            if (std::abs(dx) <= glade_radius + glade_wall_thickness 
                && std::abs(dy) <= glade_radius + glade_wall_thickness) {
                // walls outside of glade radius
                if (static_cast<int>(std::abs(dx)) == glade_radius + 1 
                    || static_cast<int>(std::abs(dy)) == glade_radius + 1) {
                    if (x != half_map_size && y != half_map_size) {
                        map[y][x] = Map::INGROWN_WALL_CUBE;
                    }
                } 
                // inner glade
                else {
                    map[y][x] = Map::GROUND_CUBE;
                }

            }
        }
    }
    std::cout << "Possible void spawnpoints in set: " << void_spawnpoints.size() << '\n';
    generate_voids(void_spawnpoints, max_voids);
}


void generate_voids(std::set<std::pair<int, int>> void_locations, const int max_voids) {
    if (void_locations.empty()) std::cout << "Alert: No voids spawned! void_locations.empty()";

    std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<size_t> dist(0, void_locations.size() - 1);

    for (int i = 0; i <= max_voids; i++) {
        auto it = void_locations.begin();
        std::advance(it, dist(rng));
        auto [y, x] = *it;
        std::cout << "Assigned void pair to map data: " << y << " " << x << '\n';
        map[y][x] = Map::VOID_CUBE;
    }
}