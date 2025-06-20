#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Define the global variables here, et mainis probleeme ei tekiks.
SDL_Texture* ground_tex = nullptr;
SDL_Texture* tree_tex = nullptr;
SDL_Texture* numbers = nullptr;


void load_textures(SDL_Renderer* renderer) {

    /* textures */
    ground_tex = IMG_LoadTexture(renderer, "resources/snowy_ground.png");
    tree_tex   = IMG_LoadTexture(renderer, "resources/snowy_tree.png");
    numbers    = IMG_LoadTexture(renderer, "resources/numbers.png");

    /* Puhasta tekstuuride 22ri et ei oleks blurry */
    SDL_SetTextureScaleMode(ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(numbers, SDL_ScaleModeNearest);

}
