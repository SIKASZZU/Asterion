#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "game.h"

// Define the global variables here, et mainis probleeme ei tekiks
SDL_Texture* cube_ground_tex = nullptr;
SDL_Texture* tree_tex = nullptr;
SDL_Texture* snowy_tree_tex = nullptr;
SDL_Texture* numbers_tex = nullptr;
SDL_Texture* wall_tex = nullptr;

const int texture_width = 16;
const int texture_height = 16;

void load_textures(SDL_Renderer* renderer) {

    /* textures */
    cube_ground_tex = IMG_LoadTexture(renderer, "resources/ground_cube.png");
    tree_tex = IMG_LoadTexture(renderer, "resources/tree.png");
    snowy_tree_tex = IMG_LoadTexture(renderer, "resources/snowy_tree.png");
    numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
    wall_tex = IMG_LoadTexture(renderer, "resources/wall_cube.png");

    /* Puhasta tekstuuride 22ri et ei oleks blurry */
    SDL_SetTextureScaleMode(cube_ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(snowy_tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(wall_tex, SDL_ScaleModeNearest);
}


void destroy_all_textures() {
    SDL_DestroyTexture(cube_ground_tex);
    SDL_DestroyTexture(tree_tex);
    SDL_DestroyTexture(snowy_tree_tex);
    SDL_DestroyTexture(numbers_tex);
    SDL_DestroyTexture(wall_tex);
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


void load_cube_wall_texture(SDL_Renderer* renderer, SDL_Texture* wall_tex,
    int map[map_size][map_size], int row, int col, SDL_Rect destTile) {

    SDL_RenderCopy(renderer, wall_tex, nullptr, &destTile);
}


void load_cube_ground_texture(SDL_Renderer* renderer,
    SDL_Texture* cube_ground_tex, SDL_Rect destTile) {
    SDL_RenderCopy(renderer, cube_ground_tex, nullptr, &destTile);
}