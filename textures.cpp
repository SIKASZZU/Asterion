#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

// Define the global variables here, et mainis probleeme ei tekiks.
SDL_Texture* ground_tex = nullptr;
SDL_Texture* tree_tex = nullptr;
SDL_Texture* numbers_tex = nullptr;

const int digit_width = 16;
const int digit_height = 16;
const int numbers_tex_y = 0;

void load_textures(SDL_Renderer* renderer) {

    /* textures */
    ground_tex  = IMG_LoadTexture(renderer, "resources/snowy_ground.png");
    tree_tex    = IMG_LoadTexture(renderer, "resources/snowy_tree.png");
    numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");

    /* Puhasta tekstuuride 22ri et ei oleks blurry */
    SDL_SetTextureScaleMode(ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest);

}

void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect at_tile) {
    if (number < 0 || number > 9) {
        std::cerr << "Invalid number: " << number << " Numbers combined not added yet" << std::endl;
        return;
    }

    // Load the numbers sprite sheet (10 digits in a row, each 16px wide)
    if (!numbers_tex) {
        SDL_Texture* numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
        SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest); // CLEAN the texture
    }

    SDL_Rect number_rect = {number * digit_width, numbers_tex_y, digit_width, digit_height};
    SDL_RenderCopy(renderer, numbers_tex, &number_rect, &at_tile);

}

