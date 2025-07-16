#include "game.h"
#include "textures.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#include <vector>
#include <iostream>
#include <unordered_map>

// Image textures that are loaded into the GPU memory, see IMG_LoadTexture
class ImageTexture : public Texture {
private:
    const char* m_resource_path;
public:
    ImageTexture(SDL_Renderer* renderer, const char* resource_path) {
        this->m_resource_path = resource_path;
        this->load_texture(renderer);
    }

    SDL_Texture* get_texture() {
        return this->m_texture;
    }

const int texture_width = 16;
const int texture_height = 16;
Uint32 last_update = SDL_GetTicks();
int last_frame = 0;
int row = 0;
int player_animation_speed = 123;

    void load_texture(SDL_Renderer* renderer) {
        this->m_texture = IMG_LoadTexture(renderer, this->m_resource_path);
        SDL_SetTextureScaleMode(this->m_texture, SDL_ScaleModeNearest);
    }
};

// do not modify this after initialization with data
// stable-adressing does not work with this
// see https://jguegant.github.io/blogs/tech/dense-hash-map.html#dense-hash-map
std::unordered_map<int, Texture> texture_map;
std::unordered_map<std::pair<int, int>, int, pair_hash> grid_vines;

const int texture_width = 16;
const int texture_height = 16;

void load_textures(SDL_Renderer* renderer) {
    // only used in render_map_numbers, sprite sheed (0-9, each 16px wide)
    texture_map[Map::NUMBER_ATLAS] = ImageTexture(renderer, "resources/numbers.png");
    texture_map[Map::TREE_VAL] = ImageTexture(renderer, "resources/tree.png");
    // notused = ImageTexture(renderer, "resources/snowy_tree.png");
    texture_map[Map::GROUND_CUBE] = ImageTexture(renderer, "resources/ground_cube.png");
    texture_map[Map::MAZE_GROUND_VAL] = ImageTexture(renderer, "resources/maze_ground_cube.png");
    texture_map[Map::SNOWY_GROUND_VAL] = ImageTexture(renderer, "resources/snowy_ground_cube.png");
    texture_map[Map::ERROR_VAL] = ImageTexture(renderer, "resources/error_cube.png");
    texture_map[Map::YELLOW_VAL] = ImageTexture(renderer, "resources/yellow_cube.png");
    texture_map[Map::BLUE_VAL] = ImageTexture(renderer, "resources/blue_cube.png");
    texture_map[Map::INGROWN_WALL_VAL] = ImageTexture(renderer, "resources/ingrown_wall_cube.png");
    // this is also used as sector 1 and 3 walls
    texture_map[Map::WALL_VAL] = ImageTexture(renderer, "resources/wall_cube.png");
    texture_map[Map::CUBE_VINE_HARD_TEX] = ImageTexture(renderer, "resources/vine_cube_hard.png");
    texture_map[Map::CUBE_VINE_MEDIUM_TEX] = ImageTexture(renderer, "resources/vine_cube_medium.png");
    texture_map[Map::CUBE_VINE_SOFT_TEX] = ImageTexture(renderer, "resources/vine_cube_soft.png");
    texture_map[456] = ImageTexture(renderer, "resources/player_animation_4x4.png");
}


void destroy_all_textures() {
    for (auto& pair : texture_map) {
        pair.second.destroy_texture();
    }
    texture_map.clear();
}


/// @brief Will render an image for numbers between [0-9] (inclusive)
///
/// If the number is outside 0-9, it is ignored, will not be rendered
/// @param renderer 
/// @param number 
/// @param at_tile 
void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect dstrect) {
    // this function could be replaced by rendering text
    // instead of a picture of
    if (number < 0 || number > 9) {
        return;
    }

    // used to select a specific number from atlas
    SDL_Rect number_rect = { number * texture_width, 0, texture_width, texture_height };
    texture_map[Map::NUMBER_ATLAS].render(renderer, &number_rect, &dstrect);
}


Texture* choose_cube_vine_texture(std::string type, std::pair<int, int> grid_pos) {
    if (grid_pos == std::pair{ -1,-1 }) {
        return &texture_map[Map::CUBE_VINE_MEDIUM_TEX];
    }

    int vine_number = grid_vines.try_emplace(grid_pos, rand() % 3)
        .first->second;

    if (type == "hard" || vine_number == 2) {
        return &texture_map[Map::CUBE_VINE_HARD_TEX];
    }
    if (type == "medium" || vine_number == 1) {
        return &texture_map[Map::CUBE_VINE_MEDIUM_TEX];
    }
    if (type == "soft" || vine_number == 0) {
        return &texture_map[Map::CUBE_VINE_SOFT_TEX];
    }
    return &texture_map[Map::CUBE_VINE_MEDIUM_TEX];
}

void load_player_sprite(SDL_Renderer* renderer) {
    const int sprite_width = 32;
    const int sprite_height = 32;

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

    texture_map[456].render(renderer, &srcRect, &dstRect);
}

/* Texture method definitions*/

void Texture::destroy_texture() {
    SDL_DestroyTexture(this->m_texture);
}

void Texture::render(SDL_Renderer* renderer, const SDL_Rect* srcrect, const SDL_Rect* dstrect) {
    SDL_RenderCopy(renderer, this->m_texture, srcrect, dstrect);
}
void Texture::render(SDL_Renderer* renderer, const SDL_Rect* dstrect) {
    SDL_RenderCopy(renderer, this->m_texture, nullptr, dstrect);
}
