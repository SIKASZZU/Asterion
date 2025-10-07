#include "game.hpp"
#include "textures.hpp"
#include "assets.hpp"
#include "render.hpp"
#include "offset.hpp"

#include <vector>
#include <iostream>
#include <unordered_map>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL.h>


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

    void load_texture(SDL_Renderer* renderer) {
        this->m_texture = IMG_LoadTexture(renderer, this->m_resource_path);
        SDL_SetTextureScaleMode(this->m_texture, SDL_SCALEMODE_NEAREST);
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
    texture_map[Map::PLAYER] = ImageTexture(renderer, Assets::Images::player_animation);
    texture_map[Map::SPIDER] = ImageTexture(renderer, Assets::Images::spider_animation);
    // only used in render_map_numbers, sprite sheed (0-9, each 16px wide)
    texture_map[Map::NUMBER_ATLAS] = ImageTexture(renderer, Assets::Images::numbers);
    texture_map[Map::TREE] = ImageTexture(renderer, Assets::Images::tree);
    texture_map[Map::TREE_TRUNK] = ImageTexture(renderer, Assets::Images::tree_trunk);
    // notused = ImageTexture(renderer, "resources/snowy_tree.png");
    texture_map[Map::GROUND_CUBE] = ImageTexture(renderer, Assets::Images::ground_cube);
    texture_map[Map::GROUND_CUBE_SPRITE] = ImageTexture(renderer, Assets::Images::ground_cube_sprite);
    texture_map[Map::MAZE_GROUND_CUBE] = ImageTexture(renderer, Assets::Images::maze_ground_cube);
    texture_map[Map::MAZE_GROUND_SPRITE] = ImageTexture(renderer, Assets::Images::maze_ground_spirte);
    // texture_map[Map::SNOWY_GROUND_CUBE] = ImageTexture(renderer, "resources/snowy_ground_cube.png");
    texture_map[Map::ERROR_CUBE] = ImageTexture(renderer, Assets::Images::error_cube);
    texture_map[Map::YELLOW_CUBE] = ImageTexture(renderer, Assets::Images::yellow_cube);
    texture_map[Map::BLUE_CUBE] = ImageTexture(renderer, Assets::Images::blue_cube);
    texture_map[Map::INGROWN_WALL_CUBE] = ImageTexture(renderer, Assets::Images::ingrown_wall_cube);
    texture_map[Map::WALL_CUBE] = ImageTexture(renderer, Assets::Images::wall_cube);
    // this is used as sector 1,2,3 walls
    texture_map[Map::WALL_CUBE_SPRITE] = ImageTexture(renderer, Assets::Images::wall_cube_sprite);
    texture_map[Map::VINE_CUBE_HARD] = ImageTexture(renderer, Assets::Images::vine_cube_hard);
    texture_map[Map::VINE_CUBE_MEDIUM] = ImageTexture(renderer, Assets::Images::vine_cube_medium);
    texture_map[Map::VINE_CUBE_SOFT] = ImageTexture(renderer, Assets::Images::vine_cube_soft);
    texture_map[Map::VINE_OVERHANG_SN] = ImageTexture(renderer, Assets::Images::vine_overhang_sn);
    texture_map[Map::VINE_OVERHANG_EW] = ImageTexture(renderer, Assets::Images::vine_overhang_ew);
    texture_map[Map::VINE_COVER_N] = ImageTexture(renderer, Assets::Images::vine_cover_n);
    texture_map[Map::VOID_CUBE] = ImageTexture(renderer, Assets::Images::void_cube);
    texture_map[Map::VOID_CUBE_NEIGHBOUR] = ImageTexture(renderer, Assets::Images::void_cube_tilemap);
    texture_map[Map::GRASS_COVER_SPRITE] = ImageTexture(renderer, Assets::Images::grass_cover_sprite);
    texture_map[Map::GRASS_COVER] = ImageTexture(renderer, Assets::Images::grass_cover);
    texture_map[Map::GRASS_COVER_SHORT] = ImageTexture(renderer, Assets::Images::grass_cover_short);
    texture_map[Map::GRASS_COVER_TALL] = ImageTexture(renderer, Assets::Images::grass_cover_tall);
    texture_map[Map::INVISIBLE_CUBE] = ImageTexture(renderer, Assets::Images::invisible_cube);
    texture_map[Map::WALL_MARKINGS] = ImageTexture(renderer, Assets::Images::wall_markings);
    texture_map[Map::MAZE_DECO] = ImageTexture(renderer, Assets::Images::maze_deco);
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
void load_specific_number(SDL_Renderer* renderer, int number, SDL_FRect dstrect) {
    // this function could be replaced by rendering text
    // instead of a picture of
    if (number < 0 || number > 9) {
        return;
    }

    // used to select a specific number from atlas
    SDL_FRect number_rect = {
        static_cast<float>(number * texture_width),
        0,
        texture_width,
        texture_height
    };
    texture_map[Map::NUMBER_ATLAS].render(renderer, &number_rect, &dstrect);
}


Texture* choose_cube_vine_texture(std::string type, std::pair<int, int> grid_pos) {
    if (grid_pos == std::pair{ -1,-1 }) {
        return nullptr;
    }

    int vine_number = grid_vines.try_emplace(grid_pos, rand() % 4)
        .first->second;

    if (type == "hard" || vine_number == 2) {
        return &texture_map[Map::VINE_CUBE_HARD];
    }
    if (type == "medium" || vine_number == 1) {
        return &texture_map[Map::VINE_CUBE_MEDIUM];
    }
    if (type == "soft" || vine_number == 0) {
        return &texture_map[Map::VINE_CUBE_SOFT];
    }
    return nullptr;
}


void render_void_tilemap(SDL_Renderer* renderer, struct Offset& offset,
    int map[map_size][map_size], std::pair<int, int> grid_pos, SDL_FRect destTile) {
    float tex_width = 32.0f;
    float tex_height = 31.0f;
    SDL_FRect srcTile = { 0, 0, tex_width, tex_height };
    auto [row, col] = grid_pos;

    if (map[row][col] == Map::VOID_CUBE) {
        // render 32, 0, tex_width, tex_height,
        // VOID_CUBE_NEIGHBOUR on 2x2 tilemap, VOID_CUBE on single tile.
        srcTile.x = 32; srcTile.y = 0;
        texture_map[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
        // change 2x2 to VOID_CUBE_NEIGHBOURS
        map[row + 1][col] = Map::VOID_CUBE_NEIGHBOUR;  // East
        map[row][col + 1] = Map::VOID_CUBE_NEIGHBOUR;  // West
        map[row + 1][col + 1] = Map::VOID_CUBE_NEIGHBOUR;  // South
        // around void, set trees to tree_trunks
        const int radius = 3;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int ny = row + dy;
                int nx = col + dx;
                // Check if the (nx, ny) is within circular radius
                if (dx * dx + dy * dy <= radius * radius) {
                    // Bounds check
                    if (ny >= 0 && ny < map_size &&
                        nx >= 0 && nx < map_size) {
                        if (map[ny][nx] == Map::TREE) {
                            random_offsets_trees.erase({ ny, nx });
                            map[ny][nx] = Map::TREE_TRUNK;
                        }
                    }
                }
            }
        }
    }
    if (map[row][col] == Map::VOID_CUBE_NEIGHBOUR) {
        // check surroudings
        if (map[row - 1][col] == VOID_CUBE) {
            srcTile.x = 0, srcTile.y = 31;
            texture_map[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
        }
        else if (map[row][col - 1] == VOID_CUBE) {
            srcTile.x = 32, srcTile.y = 31;
            texture_map[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
        }
        else if (map[row - 1][col] == VOID_CUBE_NEIGHBOUR
            && map[row][col - 1] == VOID_CUBE_NEIGHBOUR) {
            srcTile.x = 0, srcTile.y = 0;
            texture_map[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
        }
    }
}

/* Texture method definitions*/

void Texture::destroy_texture() {
    SDL_DestroyTexture(this->m_texture);
}

void Texture::render(SDL_Renderer* renderer, const SDL_FRect* srcrect, const SDL_FRect* dstrect) {
    SDL_RenderTexture(renderer, this->m_texture, srcrect, dstrect);
}
void Texture::render(SDL_Renderer* renderer, const SDL_FRect* dstrect) {
    SDL_RenderTexture(renderer, this->m_texture, nullptr, dstrect);
}
