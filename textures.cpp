#include "game.h"
#include "textures.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <unordered_map>

// Define the global variables here, et mainis probleeme ei tekiks
SDL_Texture* cube_error_tex = nullptr;
SDL_Texture* cube_ground_tex = nullptr;
SDL_Texture* cube_snowy_ground_tex = nullptr;
SDL_Texture* tree_tex = nullptr;
SDL_Texture* snowy_tree_tex = nullptr;
SDL_Texture* numbers_tex = nullptr;
SDL_Texture* cube_wall_tex = nullptr;
SDL_Texture* cube_vine_hard_tex = nullptr;
SDL_Texture* cube_vine_medium_tex = nullptr;
SDL_Texture* cube_vine_soft_tex = nullptr;
SDL_Texture* cube_yellow_tex = nullptr;
SDL_Texture* cube_blue_tex = nullptr;
SDL_Texture* cube_maze_ground_tex = nullptr;
SDL_Texture* cube_ingrown_wall_tex = nullptr;
SDL_Texture* player_animation_4x4 = nullptr;

const int texture_width = 16;
const int texture_height = 16;
Uint32 last_update = SDL_GetTicks();
int last_frame = 0;
int row = 0;
int player_animation_speed = 123;

std::unordered_map<std::pair<int, int>, int, pair_hash> grid_vines;

void load_textures(SDL_Renderer* renderer) {

    /* textures */
    cube_error_tex = IMG_LoadTexture(renderer, "resources/error_cube.png");
    cube_ground_tex = IMG_LoadTexture(renderer, "resources/ground_cube.png");
    cube_snowy_ground_tex = IMG_LoadTexture(renderer, "resources/snowy_ground_cube.png");
    tree_tex = IMG_LoadTexture(renderer, "resources/tree.png");
    snowy_tree_tex = IMG_LoadTexture(renderer, "resources/snowy_tree.png");
    numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
    cube_wall_tex = IMG_LoadTexture(renderer, "resources/wall_cube.png");
    cube_vine_hard_tex = IMG_LoadTexture(renderer, "resources/vine_cube_hard.png");
    cube_vine_medium_tex = IMG_LoadTexture(renderer, "resources/vine_cube_medium.png");
    cube_vine_soft_tex = IMG_LoadTexture(renderer, "resources/vine_cube_soft.png");
    cube_yellow_tex = IMG_LoadTexture(renderer, "resources/yellow_cube.png");
    cube_blue_tex = IMG_LoadTexture(renderer, "resources/blue_cube.png");
    cube_maze_ground_tex = IMG_LoadTexture(renderer, "resources/maze_ground_cube.png");
    cube_ingrown_wall_tex = IMG_LoadTexture(renderer, "resources/ingrown_wall_cube.png");
    player_animation_4x4 = IMG_LoadTexture(renderer, "resources/player_animation_4x4.png");

    /* Puhasta tekstuuride 22ri et ei oleks blurry */
    SDL_SetTextureScaleMode(cube_error_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_snowy_ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(snowy_tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_wall_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_vine_hard_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_vine_medium_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_vine_soft_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_yellow_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_blue_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_maze_ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(cube_ingrown_wall_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(player_animation_4x4, SDL_ScaleModeNearest);
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
    SDL_DestroyTexture(cube_yellow_tex);
    SDL_DestroyTexture(cube_blue_tex);
    SDL_DestroyTexture(cube_maze_ground_tex);
    SDL_DestroyTexture(cube_ingrown_wall_tex);
    SDL_DestroyTexture(player_animation_4x4);
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
void load_cube_maze_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_maze_ground_tex, nullptr, &destTile); }
void load_cube_snowy_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_snowy_ground_tex, nullptr, &destTile); }
void load_cube_error_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_error_tex, nullptr, &destTile); }
void load_cube_yellow_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_yellow_tex, nullptr, &destTile); }
void load_cube_blue_texture(SDL_Renderer* renderer, SDL_Rect destTile) { SDL_RenderCopy(renderer, cube_blue_tex, nullptr, &destTile); }

SDL_Texture* choose_cube_vine_texture(std::string type, std::pair<int, int> grid_pos) {
    SDL_Texture* tex;

    if (grid_pos == std::pair{ -1,-1 }) {
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



void load_player_sprite(SDL_Renderer* renderer) {
    const int sprite_width = 31;  // 0,0 kaasaarvatud
    const int sprite_height = 31;

    SDL_Rect srcRect;
    SDL_Rect dstRect = {
        static_cast<int>(player.rect.x),
        static_cast<int>(player.rect.y),
        static_cast<int>(player.rect.w),
        static_cast<int>(player.rect.h)
    };

    // Decide the row based on movement direction
    if (player.movement_vector.x == 1)  { row = 0; }
    if (player.movement_vector.x == -1) { row = 1; }
    if (player.movement_vector.y == 1)  { row = 2; }
    if (player.movement_vector.y == -1) { row = 3; }

    int col = last_frame % 4;  // loop 0-3 for frames
    srcRect.x = col * sprite_width;
    srcRect.y = row * sprite_height;
    srcRect.w = sprite_width;
    srcRect.h = sprite_height;

    if (SDL_GetTicks() - last_update > player_animation_speed) {
        last_frame++;
        last_update = SDL_GetTicks();
    }

    SDL_RenderCopy(renderer, player_animation_4x4, &srcRect, &dstRect);
}