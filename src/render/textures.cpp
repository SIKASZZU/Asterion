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
    const char* mResourcePath;
public:
    ImageTexture(SDL_Renderer* renderer, const char* resourcePath) {
        this->mResourcePath = resourcePath;
        this->load_texture(renderer);
    }

    SDL_Texture* get_texture() {
        return this->mTexture;
    }

    void load_texture(SDL_Renderer* renderer) {
        this->mTexture = IMG_LoadTexture(renderer, this->mResourcePath);
        SDL_SetTextureScaleMode(this->mTexture, SDL_SCALEMODE_NEAREST);
    }
};

// do not modify this after initialization with data
// stable-adressing does not work with this
// see https://jguegant.github.io/blogs/tech/dense-hash-map.html#dense-hash-map
std::unordered_map<int, Texture> textureMap;
std::unordered_map<std::pair<int, int>, int, pair_hash> gridVines;

void load_textures(SDL_Renderer* renderer) {
    textureMap[Map::PLAYER] = ImageTexture(renderer, Assets::Images::player_animation);
    // textureMap[Map::SPIDER] = ImageTexture(renderer, Assets::Images::spider_animation);
    // spider run 8-way
    textureMap[Map::spider_run45_animation] = ImageTexture(renderer, Assets::Images::spider_run45_animation);
    textureMap[Map::spider_run135_animation] = ImageTexture(renderer, Assets::Images::spider_run135_animation);
    textureMap[Map::spider_run225_animation] = ImageTexture(renderer, Assets::Images::spider_run225_animation);
    textureMap[Map::spider_run315_animation] = ImageTexture(renderer, Assets::Images::spider_run315_animation);
    textureMap[Map::spider_run90_animation] = ImageTexture(renderer, Assets::Images::spider_run90_animation);
    textureMap[Map::spider_run180_animation] = ImageTexture(renderer, Assets::Images::spider_run180_animation);
    textureMap[Map::spider_run270_animation] = ImageTexture(renderer, Assets::Images::spider_run270_animation);
    textureMap[Map::spider_run0_animation] = ImageTexture(renderer, Assets::Images::spider_run0_animation);
    // spider idle 8-way
    textureMap[Map::spider_idle45_animation] = ImageTexture(renderer, Assets::Images::spider_idle45_animation);
    textureMap[Map::spider_idle135_animation] = ImageTexture(renderer, Assets::Images::spider_idle135_animation);
    textureMap[Map::spider_idle225_animation] = ImageTexture(renderer, Assets::Images::spider_idle225_animation);
    textureMap[Map::spider_idle315_animation] = ImageTexture(renderer, Assets::Images::spider_idle315_animation);
    textureMap[Map::spider_idle90_animation] = ImageTexture(renderer, Assets::Images::spider_idle90_animation);
    textureMap[Map::spider_idle180_animation] = ImageTexture(renderer, Assets::Images::spider_idle180_animation);
    textureMap[Map::spider_idle270_animation] = ImageTexture(renderer, Assets::Images::spider_idle270_animation);
    textureMap[Map::spider_idle0_animation] = ImageTexture(renderer, Assets::Images::spider_idle0_animation);
    textureMap[Map::TREE] = ImageTexture(renderer, Assets::Images::tree);
    textureMap[Map::TREE_TRUNK] = ImageTexture(renderer, Assets::Images::tree_trunk);
    // notused = ImageTexture(renderer, "resources/snowy_tree.png");
    textureMap[Map::GROUND_CUBE] = ImageTexture(renderer, Assets::Images::ground_cube);
    textureMap[Map::GROUND_CUBE_SPRITE] = ImageTexture(renderer, Assets::Images::ground_cube_sprite);
    textureMap[Map::MAZE_GROUND_CUBE] = ImageTexture(renderer, Assets::Images::maze_ground_cube);
    textureMap[Map::MAZE_GROUND_SPRITE] = ImageTexture(renderer, Assets::Images::maze_ground_spirte);
    // textureMap[Map::SNOWY_GROUND_CUBE] = ImageTexture(renderer, "resources/snowy_ground_cube.png");
    textureMap[Map::ERROR_CUBE] = ImageTexture(renderer, Assets::Images::error_cube);
    textureMap[Map::YELLOW_CUBE] = ImageTexture(renderer, Assets::Images::yellow_cube);
    textureMap[Map::BLUE_CUBE] = ImageTexture(renderer, Assets::Images::blue_cube);
    textureMap[Map::INGROWN_WALL_CUBE] = ImageTexture(renderer, Assets::Images::ingrown_wall_cube);
    textureMap[Map::WALL_CUBE] = ImageTexture(renderer, Assets::Images::wall_cube);
    textureMap[Map::WALL_CUBE_GROUND] = ImageTexture(renderer, Assets::Images::wall_cube_ground);
    // this is used as sector 1,2,3 walls
    textureMap[Map::WALL_CUBE_SPRITE] = ImageTexture(renderer, Assets::Images::wall_cube_sprite);
    textureMap[Map::VINE_CUBE_HARD] = ImageTexture(renderer, Assets::Images::vine_cube_hard);
    textureMap[Map::VINE_CUBE_MEDIUM] = ImageTexture(renderer, Assets::Images::vine_cube_medium);
    textureMap[Map::VINE_CUBE_SOFT] = ImageTexture(renderer, Assets::Images::vine_cube_soft);
    textureMap[Map::VINE_OVERHANG_SN] = ImageTexture(renderer, Assets::Images::vine_overhang_sn);
    textureMap[Map::VINE_OVERHANG_EW] = ImageTexture(renderer, Assets::Images::vine_overhang_ew);
    textureMap[Map::VINE_COVER_N] = ImageTexture(renderer, Assets::Images::vine_cover_n);
    textureMap[Map::VOID_CUBE] = ImageTexture(renderer, Assets::Images::void_cube);
    textureMap[Map::VOID_CUBE_NEIGHBOUR] = ImageTexture(renderer, Assets::Images::void_cube_tilemap);
    textureMap[Map::GRASS_COVER_SPRITE] = ImageTexture(renderer, Assets::Images::grass_cover_sprite);
    textureMap[Map::GRASS_COVER] = ImageTexture(renderer, Assets::Images::grass_cover);
    textureMap[Map::GRASS_COVER_SHORT] = ImageTexture(renderer, Assets::Images::grass_cover_short);
    textureMap[Map::GRASS_COVER_TALL] = ImageTexture(renderer, Assets::Images::grass_cover_tall);
    textureMap[Map::INVISIBLE_CUBE] = ImageTexture(renderer, Assets::Images::invisible_cube);
    textureMap[Map::WALL_MARKINGS] = ImageTexture(renderer, Assets::Images::wall_markings);
    textureMap[Map::MAZE_DECO] = ImageTexture(renderer, Assets::Images::maze_deco);
}

void destroy_all_textures() {
    for (auto& pair : textureMap) {
        pair.second.destroy_texture();
    }
    textureMap.clear();
}

Texture* choose_cube_vine_texture(std::string type, std::pair<int, int> gridPos) {
    if (gridPos == std::pair{ -1,-1 }) {
        return nullptr;
    }

    int vineNumber = gridVines.try_emplace(gridPos, rand() % 4)
        .first->second;

    if (type == "hard" || vineNumber == 2) {
        return &textureMap[Map::VINE_CUBE_HARD];
    }
    if (type == "medium" || vineNumber == 1) {
        return &textureMap[Map::VINE_CUBE_MEDIUM];
    }
    if (type == "soft" || vineNumber == 0) {
        return &textureMap[Map::VINE_CUBE_SOFT];
    }
    return nullptr;
}

void render_void_tilemap(SDL_Renderer* renderer, struct Offset& offset,
    int map[mapSize][mapSize], std::pair<int, int> gridPos, SDL_FRect destTile) {
    SDL_FRect srcTile = { 0, 0, Texture::spriteWidth, Texture::spriteHeight };
    auto [row, col] = gridPos;

    if (map[row][col] == Map::VOID_CUBE) {
        // VOID_CUBE_NEIGHBOUR on 2x2 tilemap, VOID_CUBE on single tile.
        srcTile.x = 32; srcTile.y = 0;
        textureMap[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
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
                    if (ny >= 0 && ny < mapSize &&
                        nx >= 0 && nx < mapSize) {
                        if (map[ny][nx] == Map::TREE) {
                            randomOffsetsTrees.erase({ ny, nx });
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
            srcTile.x = 0, srcTile.y = 32;
            textureMap[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
        }
        else if (map[row][col - 1] == VOID_CUBE) {
            srcTile.x = 32, srcTile.y = 32;
            textureMap[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
        }
        else if (map[row - 1][col] == VOID_CUBE_NEIGHBOUR
            && map[row][col - 1] == VOID_CUBE_NEIGHBOUR) {
            srcTile.x = 0, srcTile.y = 0;
            textureMap[Map::VOID_CUBE_NEIGHBOUR].render(renderer, &srcTile, &destTile);
        }
    }
}

/* Texture method definitions*/

void Texture::destroy_texture() {
    SDL_DestroyTexture(this->mTexture);
}

void Texture::render(SDL_Renderer* renderer, const SDL_FRect* srcrect, const SDL_FRect* dstrect) {
    SDL_RenderTexture(renderer, this->mTexture, srcrect, dstrect);
}
void Texture::render(SDL_Renderer* renderer, const SDL_FRect* dstrect) {
    SDL_RenderTexture(renderer, this->mTexture, nullptr, dstrect);
}
