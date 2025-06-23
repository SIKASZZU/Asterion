#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "game.h"
#include <map>

// Define the global variables here, et mainis probleeme ei tekiks
SDL_Texture* ground_tex = nullptr;
SDL_Texture* snowy_ground_tex = nullptr;
SDL_Texture* tree_tex = nullptr;
SDL_Texture* numbers_tex = nullptr;
SDL_Texture* wall_tex = nullptr;

const int texture_width = 16;
const int texture_height = 16;

std::string ground_tex_path = " ";
static std::map<std::pair<int, int>, std::string> row_columnToPath;
static std::map<std::string, SDL_Texture*> texture_cache;


void load_textures(SDL_Renderer* renderer) {

    /* folder paths -> funciga leiad / randomly / yhe pildi folderist */
    std::string ground_tex_path = "resources/Ground/Ground_";
    
    /* textures */
    snowy_ground_tex  = IMG_LoadTexture(renderer, "resources/ground_cube.png");
    tree_tex    = IMG_LoadTexture(renderer, "resources/snowy_tree.png");
    numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
    wall_tex    = IMG_LoadTexture(renderer, "resources/wall_cube.png");

    /* Puhasta tekstuuride 22ri et ei oleks blurry */
    SDL_SetTextureScaleMode(snowy_ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(wall_tex, SDL_ScaleModeNearest);

}


void destroy_all_textures() {
    SDL_DestroyTexture(ground_tex);
    SDL_DestroyTexture(snowy_ground_tex);
    SDL_DestroyTexture(tree_tex);
    SDL_DestroyTexture(numbers_tex);
    SDL_DestroyTexture(wall_tex);
}


void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect at_tile) {
    if (number < 0 || number > 9) {
        std::cerr << "Invalid number: " << number << " Numbers combined not added yet" << std::endl;
        return;
    }

    // Load the numbers sprite sheet (0 - 9, each 16px wide)
    if (!numbers_tex) {
        SDL_Texture* numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
        SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest); // CLEAN the texture
    }

    SDL_Rect number_rect = {number * texture_width, 0, texture_width, texture_height};
    SDL_RenderCopy(renderer, numbers_tex, &number_rect, &at_tile);
}


void draw_wall(SDL_Renderer* renderer, SDL_Texture* wall_tex, const bool neighbors[3][3], SDL_Rect destTile) {
    /* Summa (var int mask) j2rgi t6mbab maskToXY arraist walli info, et otsida seda tekstuurilt */
 
    int mask = 0;
    if (neighbors[0][1]) mask |= 1;  // North
    if (neighbors[1][2]) mask |= 2;  // East
    if (neighbors[2][1]) mask |= 4;  // South
    if (neighbors[1][0]) mask |= 8;  // West

    // Define how each mask maps to (column, row) in your texture.
    // Adjust the pairs if your actual texture layout differs.
    static const std::pair<int,int> maskToXY[16] = {
                  /* !!!!!KUST POOL CONNECTION TEKIB!!!!!! vahel on vale idk */
        {0, 0},   // 0: single block
        {4, 2},   // 1: north XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {5, 0},   // 2: west XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {1, 2},   // 3: west+north XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {4, 0},   // 4: south XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {4, 1},   // 5: north+south
        {1, 0},   // 6: west+south
        {4, 4},   // 7: north+west+south
        {7, 0},   // 8: east XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {3, 2},   // 9: east+north XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {6, 0},   // 10: east+west (span)
        {1, 4},   // 11: north+east+west
        {3, 0},   // 12: east+south XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 
        {10, 4},   // 13: north+south+east XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {7, 4},   // 14: east+south+west Y
        {9, 1}    // 15: all four sides connected (intersection)
        
    };

    auto [col, row] = maskToXY[mask];
    SDL_Rect src = {
        col * 16,      // x-offset in texture
        row * 16,      // y-offset in texture
        16, 16         // width & height of each tile
    };
    SDL_RenderCopy(renderer, wall_tex, &src, &destTile);
}


void load_wall_texture(SDL_Renderer* renderer, SDL_Texture* wall_tex, int map[map_size][map_size], int row, int col, SDL_Rect destTile) {
    SDL_RenderCopy(renderer, wall_tex, nullptr, &destTile);
    // bool neighbors[3][3] = {{false}};
    // for (int dr = -1; dr <= 1; ++dr) {
    //     for (int dc = -1; dc <= 1; ++dc) {
    //         int r = row + dr, c = col + dc;
    //         if (r >= 0 && r < 100 && c >= 0 && c < 100 && map[r][c] == 9) {
    //             neighbors[dr + 1][dc + 1] = true;
    //         }
    //     }
    // }
    // neighbors[1][1] = true; // always set center true

    // /* print out wall neighbours */
    // // for (int dr = 0; dr < 3; ++dr) {
    // //     for (int dc = 0; dc < 3; ++dc) {
    // //         std::cout << (neighbors[dr][dc] ? '1' : '0') << ' ';
    // //     }
    // //     std::cout << '\n';
    // // }
    // // std::cout << "---\n";

    // draw_wall(renderer, wall_tex, neighbors, destTile);
}


SDL_Texture* get_ground_texture(SDL_Renderer* renderer, const std::string& path) {
    static std::map<std::string, SDL_Texture*> texture_cache;

    auto it = texture_cache.find(path);
    if (it != texture_cache.end()) {
        return it->second; // already loaded
    }

    // Load and cache
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex) {
        std::cerr << "Failed to load texture: " << path << "\n";
    } else {
        texture_cache[path] = tex;
    }
    return tex;
}


void load_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile, int row, int column) {
    static std::map<std::pair<int, int>, std::string> row_columnToPath;
    std::pair<int, int> coord = {row, column};

    if (row_columnToPath.find(coord) == row_columnToPath.end()) {
        int random_number = random_number_gen(1, 20);
        std::string path = "resources/Ground/Ground_" + std::to_string(random_number) + ".png";
        row_columnToPath[coord] = path;
    }

    const std::string& path = row_columnToPath[coord];
    SDL_Texture* ground_tex = get_ground_texture(renderer, path);
    
    if (ground_tex) {
        SDL_RenderCopy(renderer, ground_tex, nullptr, &destTile);
    }
}


void load_snowy_ground_texture(SDL_Renderer* renderer, SDL_Texture* snowy_ground_tex, SDL_Rect destTile) {
    SDL_RenderCopy(renderer, snowy_ground_tex, nullptr, &destTile);
}


void free_ground_textures() {
    extern std::map<std::string, SDL_Texture*> texture_cache;
    for (auto& [_, tex] : texture_cache) {
        SDL_DestroyTexture(tex);
    }
    texture_cache.clear();
}