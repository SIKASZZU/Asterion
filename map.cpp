#include "map.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <algorithm>

#ifndef M_PI
#   define M_PI 3.1415926535897932384626433832
#endif

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
    /* Vali ise, mis mappi tahad geneda.    */
    // generate_random_map(map, 2, 2);
    generate_maze_runner_map(map);
}


void generate_random_map(int map[map_size][map_size], int min_val, int max_val) {
    srand(static_cast<unsigned int>(std::time(NULL)));

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

            // Clamp to 0–1
            if (land_chance < 0.0f) land_chance = 0.0f;
            if (land_chance > 1.0f) land_chance = 1.0f;

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
    srand(static_cast<unsigned int>(std::time(NULL)));

    int MIN_VAL = 0;
    int TREE_VAL = 2;
    int WALL_VAL = 9;
    int LAND_VAL = 1;

    float center_x = map_size / 2.0f;
    float center_y = map_size / 2.0f;
    float max_distance = std::sqrt(center_x * center_x + center_y * center_y);

    /* Glade */
    int glade_radius = map_size / 10; // Central square half-size
    int maze_inner_radius = glade_radius + 2;
    int maze_outer_radius = map_size / 2 - 4;

    /* Walls surrouding the Glade */
    float tree_ring_radius = glade_radius + 1.0f;
    float radius_sq_min = (tree_ring_radius - 0.5f) * (tree_ring_radius - 0.5f);
    float radius_sq_max = (tree_ring_radius + 1.5f) * (tree_ring_radius + 1.5f);

    int num_sectors = 8;

    for (int y = 0; y < map_size; y++) {
        for (int x = 0; x < map_size; x++) {
            float dx = x - center_x;
            float dy = y - center_y;

            float distance_sq = dx * dx + dy * dy;

            float distance = std::sqrt(distance_sq);

            // Default terrain logic
            float norm_dist = distance / max_distance;
            float land_chance = 1.0f - norm_dist;
            land_chance += ((rand() % 100) / 100.0f - 0.5f) * 0.8f;
            land_chance = std::clamp(land_chance, 0.0f, 1.0f);

            // Land under everything (circular)
            if (land_chance > 0.4f) {
                // int LAND_VAL = MIN_VAL + (max_val - MIN_VAL) / 2 + rand() % ((max_val - MIN_VAL) / 2 + 1);
                map[y][x] = LAND_VAL;
            }
            else {
                map[y][x] = TREE_VAL; // water
            }

            // --- STRUCTURES OVERLAY ---

            // Glade (central square)
            if (abs(x - center_x) <= glade_radius && abs(y - center_y) <= glade_radius) {
                map[y][x] = 5;
            }

            // Maze ring (pindala)
            if (distance >= maze_inner_radius && distance <= maze_outer_radius) {
                map[y][x] = 4;
            }

            // Walls around Glade
            if (distance_sq >= radius_sq_min && distance_sq <= radius_sq_max) {
                map[y][x] = WALL_VAL;
            }

            // Sector walls
            if (distance >= maze_inner_radius && distance <= maze_outer_radius) {
                float angle = std::atan2(dy, dx);
                if (angle < 0) angle += 2 * M_PI;
                float sector_angle = 2 * M_PI / num_sectors;

                for (int s = 0; s < num_sectors; ++s) {
                    float wall_angle = s * sector_angle;
                    if (std::fabs(angle - wall_angle) < 0.02) {
                        map[y][x] = WALL_VAL;
                    }
                }
            }
        }
    }
}