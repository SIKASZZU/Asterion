#include "game.h"
#include "textures.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <unordered_map>

// Define the global variables here, et mainis probleeme ei tekiks
SDL_Texture* cube_ground_tex = nullptr;
SDL_Texture* cube_snowy_ground_tex = nullptr;
SDL_Texture* tree_tex = nullptr;
SDL_Texture* snowy_tree_tex = nullptr;
SDL_Texture* numbers_tex = nullptr;
SDL_Texture* cube_wall_tex = nullptr;
SDL_Texture* cube_vine_hard_tex = nullptr;
SDL_Texture* cube_vine_medium_tex = nullptr;
SDL_Texture* cube_vine_soft_tex = nullptr;

const int texture_width = 16;
const int texture_height = 16;

std::vector<SDL_Texture*> texture_vector = {};
std::unordered_map<std::pair<int, int>, int, pair_hash> grid_vines;

void load_textures(SDL_Renderer* renderer) {

    /* textures */
    cube_ground_tex = IMG_LoadTexture(renderer, "resources/ground_cube.png");
    cube_snowy_ground_tex = IMG_LoadTexture(renderer, "resources/snowy_ground_cube.png");
    tree_tex = IMG_LoadTexture(renderer, "resources/tree.png");
    snowy_tree_tex = IMG_LoadTexture(renderer, "resources/snowy_tree.png");
    numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
    cube_wall_tex = IMG_LoadTexture(renderer, "resources/wall_cube.png");
    cube_vine_hard_tex = IMG_LoadTexture(renderer, "resources/vine_cube_hard.png");
    cube_vine_medium_tex = IMG_LoadTexture(renderer, "resources/vine_cube_medium.png");
    cube_vine_soft_tex = IMG_LoadTexture(renderer, "resources/vine_cube_soft.png");

    /* Puhasta tekstuuride 22ri et ei oleks blurry */
    SDL_SetTextureScaleMode(cube_ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_snowy_ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(snowy_tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_wall_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_vine_hard_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_vine_medium_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_vine_soft_tex, SDL_ScaleModeNearest);

    /* save textures to list so exporting is easier! */
    texture_vector.push_back(cube_ground_tex);
    texture_vector.push_back(cube_snowy_ground_tex);
    texture_vector.push_back(tree_tex);
    texture_vector.push_back(snowy_tree_tex);
    texture_vector.push_back(numbers_tex);
    texture_vector.push_back(cube_wall_tex);
    texture_vector.push_back(cube_vine_hard_tex);
    texture_vector.push_back(cube_vine_medium_tex);
    texture_vector.push_back(cube_vine_soft_tex);
}


void destroy_all_textures() {
    SDL_DestroyTexture(cube_ground_tex);
    SDL_DestroyTexture(cube_snowy_ground_tex);
    SDL_DestroyTexture(tree_tex);
    SDL_DestroyTexture(snowy_tree_tex);
    SDL_DestroyTexture(numbers_tex);
    SDL_DestroyTexture(cube_wall_tex);
    SDL_DestroyTexture(cube_vine_hard_tex);
    SDL_DestroyTexture(cube_vine_medium_tex);
    SDL_DestroyTexture(cube_vine_soft_tex);
}


void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect at_tile) {
    if (number < 0 || number > 9) {
        return;
    }

    // Load the numbers sprite sheet (0 - 9, each 16px wide)
    if (!numbers_tex) {
        SDL_Texture* numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
        SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest); // CLEAN the texture
    }

    SDL_Rect number_rect = { number * texture_width, 0, texture_width, texture_height };
    SDL_RenderCopy(renderer, numbers_tex, &number_rect, &at_tile);
}


void load_cube_wall_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_wall_tex, nullptr, &destTile); }
void load_cube_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_ground_tex, nullptr, &destTile); }
void load_cube_snowy_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_snowy_ground_tex, nullptr, &destTile); }

SDL_Texture* choose_cube_vine_texture(std::string type, std::pair<int, int> grid_pos) {
    SDL_Texture* tex;
    
    if (grid_pos == std::pair{-1,-1}) {
        return tex = cube_vine_medium_tex;
    }

    if (grid_vines.find(grid_pos) == grid_vines.end()) {
        grid_vines[grid_pos] = rand() % 3;
    }

    int vine_number = grid_vines[grid_pos];

    if (type == "hard" || vine_number == 2) {
        tex = cube_vine_hard_tex;
    }
    else if (type == "medium" || vine_number == 1) {
        tex = cube_vine_medium_tex;
    }
    else if (type == "soft" || vine_number == 0) {
        tex = cube_vine_soft_tex;
    }
    else {
        tex = cube_vine_medium_tex;
    }

    return tex;
}