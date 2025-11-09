#include "textures.hpp"
#include "collision.hpp"
#include "render.hpp"
#include "isometric_calc.hpp"
#include "raycast.hpp"
#include "mod_map_data.hpp"
#include "animation.hpp"

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <unordered_set>
#include <utility>
#include <set>


RenderQueueItem::RenderQueueItem(int renderOrder, SDL_FRect dstrect, Texture* texture, float alpha = 1.0f) {
    this->renderOrder = renderOrder;
    this->dstrect = dstrect;
    this->texture = texture;
    this->alpha = alpha;
}
RenderQueueItem::RenderQueueItem(int renderOrder, SDL_FRect srcrect, SDL_FRect dstrect, Texture* texture, float alpha = 1.0f) {
    this->renderOrder = renderOrder;
    this->dstrect = dstrect;
    this->srcrect = srcrect;
    this->texture = texture;
    this->alpha = alpha;
}
RenderQueueItem::RenderQueueItem(int renderOrder, std::function<void(SDL_Renderer* renderer)> customRender) {
    this->renderOrder = renderOrder;
    this->customRender = customRender;
}

void RenderQueueItem::call_render(SDL_Renderer* renderer) {
    if (this->texture.has_value()) {
        SDL_SetTextureAlphaMod((*texture)->get_texture(), static_cast<Uint8>(alpha * 255));
        if (srcrect.h != 0 && srcrect.w != 0) {
            return this->texture.value()->render(renderer, &this->srcrect, &this->dstrect);
        }
        else {
            return this->texture.value()->render(renderer, &this->dstrect);
        }
    }
    if (this->customRender.has_value()) {
        return this->customRender.value()(renderer);
    }
    throw std::logic_error("texture or render function needed for rendering");
}

std::unordered_map<std::pair<int, int>, int, pair_hash> randomOffsetsWalls;
std::unordered_map<std::pair<int, int>, int, pair_hash> randomOffsetsTrees;
std::unordered_map<std::pair<int, int>, int, pair_hash> grassCoverGrids;
std::unordered_map<std::pair<int, int>, int, pair_hash> mazeDecoMap;
std::unordered_map<std::pair<int, int>, int, pair_hash> mazeGroundMap;
std::unordered_map<std::pair<int, int>, int, pair_hash> groundMap;
std::unordered_map<std::pair<int, int>, int, pair_hash> grassCoverMap;
std::set<std::pair<int, int>> gridVineChecked;
std::vector<RenderQueueItem> renderQueue;

SDL_FRect TerrainClass::return_destTile(int row, int column) {
    SDL_FPoint isometricCoordinates = to_isometric_grid_coordinate(column, row);
    return SDL_FRect{ isometricCoordinates.x, isometricCoordinates.y, tileSize, tileSize };
}
bool TerrainClass::is_grid_not_renderable(std::pair<int, int> gridPos) {
    if (r_pressed == false) {}
    else if (Raycast::endpointActiveGrids.find(gridPos) == Raycast::endpointActiveGrids.end()) {
        if (std::find(Raycast::decayGrids.begin(), Raycast::decayGrids.end(), gridPos)
            == Raycast::decayGrids.end()) {
            return true;
        }
        return false;
    }
    return false;
}
float TerrainClass::determine_alpha(std::pair<int, int> gridPos) {
    int row = gridPos.first;
    int column = gridPos.second;
    int maxDist = 2;
    bool inFront = ((row > playerTileY - 1 && row <= playerTileY + maxDist)
        &&
        (column > playerTileX - 1 && column <= playerTileX + maxDist));
    float alpha = 1.0f;
    if (inFront) {
        alpha = 1.0f;
    }
    return alpha;
}
int TerrainClass::create_random_grass(std::pair<int, int> gridPos, int gridValue) {
    int varIndex = grassCoverGrids.try_emplace(gridPos, rand() % 4)
        .first->second;
    if (varIndex == 0) {
        return Map::GRASS_COVER_SHORT;
    }
    else if (varIndex == 1) {
        return Map::GRASS_COVER_TALL;
    }
    else if (varIndex == 2) {
        return Map::GRASS_COVER;
    }
    else {
        return -1;
    }
}
SDL_FRect TerrainClass::return_src_1x3(std::pair<int, int> gridPos, std::unordered_map<std::pair<int, int>, int, pair_hash>& map) {
    int varIndex = map.try_emplace(gridPos, rand() % 4)
        .first->second;

    if (varIndex == 0) {
        return SDL_FRect{ 0, 0, Texture::spriteWidth, Texture::spriteHeight };
    }
    else if (varIndex == 1) {
        return SDL_FRect{ 32, 0, Texture::spriteWidth, Texture::spriteHeight };
    }
    else if (varIndex == 2) {
        return SDL_FRect{ 64, 0, Texture::spriteWidth, Texture::spriteHeight };
    }
    else {
        return SDL_FRect{ 0, 0, 0, 0 };
    }
}

void TerrainClass::calculate_miscellaneous() {
    halfTile = tileSize / 2;
    renderQueue.reserve(4 * (renderRadius * renderRadius));
    playerTileX = static_cast<int>((player.x + player.size / 2) / tileSize);
    playerTileY = static_cast<int>((player.y + player.size / 2) / tileSize);

    mapIndexLeft = std::max(0, playerTileX - renderRadius);
    mapIndexTop = std::max(0, playerTileY - renderRadius);
    mapIndexRight = std::min(mapSize - 1, playerTileX + renderRadius);
    mapIndexBottom = std::min(mapSize - 1, playerTileY + renderRadius);

}
void TerrainClass::render_ground(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row < mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column < mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect;

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };

            // if (is_grid_not_renderable(gridPos)) continue;
            if (groundValues.find(gridValue) != groundValues.end()) {
                srcFRect = return_src_1x3(gridPos, groundMap);
                if (isEmpty(srcFRect)) {
                    textureMap[Map::GROUND_CUBE].render(renderer, &destTile);
                }
                else {
                    textureMap[Map::GROUND_CUBE_SPRITE].render(renderer, &srcFRect, &destTile);
                }
            }
            if (wallValues.find(gridValue) != wallValues.end()) {
                textureMap[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
            }
            if (gridValue == Map::VINE_OVERHANG_SN || gridValue == Map::VINE_OVERHANG_EW) {
                SDL_FRect srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                textureMap[Map::MAZE_GROUND_SPRITE].render(renderer, &srcFRect, &destTile);
            }
            switch (gridValue) {
            case Map::GROUND_CUBE: {
                destTile.y -= halfTile;
                srcFRect = return_src_1x3(gridPos, grassCoverMap);
                if (!isEmpty(srcFRect)) textureMap[Map::GRASS_COVER_SPRITE].render(renderer, &srcFRect, &destTile);
                // int grassIndex = create_random_grass(gridPos, gridValue);
                // if (grassIndex != -1) textureMap[grassIndex].render(renderer, &destTile);
                break;
            }
            case Map::MAZE_GROUND_CUBE: {
                // TODO: lammuta see lahti. Deco sitt decosse.
                srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                if (isEmpty(srcFRect)) textureMap[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
                else textureMap[Map::MAZE_GROUND_SPRITE].render(renderer, &srcFRect, &destTile);
                srcFRect = return_src_1x3(gridPos, mazeDecoMap);
                if (isEmpty(srcFRect)) break;
                destTile.y -= halfTile;
                textureMap[Map::MAZE_DECO].render(renderer, &srcFRect, &destTile);
                break;
            }
            case Map::VOID_CUBE:
            case Map::VOID_CUBE_NEIGHBOUR: {
                render_void_tilemap(renderer, offset, map, gridPos, destTile);
                break;
            }
            }
        }
    }
}
void TerrainClass::render_walls() {
    for (int row = mapIndexTop; row < mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column < mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect;

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };
            alpha = determine_alpha(gridPos);

            switch (gridValue) {
                // case Map::INGROWN_WALL_CUBE: {
                //     destTile.y -= halfTile;
                //     destTile.y -= 5;
                //     destTile.x -= 5;
                //     destTile.w += 5;
                //     destTile.h += 5;
                //     renderQueue.push_back(
                //         RenderQueueItem(destTile.y, destTile, &textureMap[gridValue], alpha)
                //     );
                //     destTile.y -= halfTile;
                //     srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                //     if (!isEmpty(srcFRect)) {
                //         renderQueue.push_back(
                //             RenderQueueItem(destTile.y + halfTile + 1, srcFRect, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                //         );
                //     }
                //     else {
                //         renderQueue.push_back(
                //             RenderQueueItem(destTile.y + halfTile + 1, destTile, &textureMap[Map::WALL_CUBE], alpha)
                //         );
                //     }
                //     break;
                // }
            case Map::WALL_CUBE:
            case Map::SECTOR_1_WALL_VAL: {
                destTile.y -= halfTile;
                srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                if (!isEmpty(srcFRect)) {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y, srcFRect, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                    );
                }
                else {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y, srcFRect, destTile, &textureMap[Map::WALL_CUBE], alpha)
                    );
                }
                // // wall marking nr 1 (100 divided by % number, 20% chance)
                // if (gridValue == Map::SECTOR_1_WALL_VAL) {
                //     int varIndex = mazeDecoMap.try_emplace(gridPos, rand() % 5).first->second;
                //     if (varIndex == 4) {
                //         srcFRect = { 0, 0, 32, 32 };
                //         renderQueue.push_back(
                //             RenderQueueItem(destTile.y + 1, srcFRect, destTile, &textureMap[Map::WALL_MARKINGS], alpha)
                //         );
                //     }
                // }
                break;
            }
            case Map::SECTOR_2_WALL_VAL: {
                // create walls
                destTile.y -= halfTile;
                int xr = randomOffsetsWalls.try_emplace(gridPos, rand() % 30)
                    .first->second;
                destTile.y += (xr / 2.5);
                destTile.x += (xr / 4);
                destTile.w -= (xr / 2);
                destTile.h -= (xr / 2);
                srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                if (!isEmpty(srcFRect)) {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y, srcFRect, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                    );
                }
                else {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y, srcFRect, destTile, &textureMap[Map::WALL_CUBE], alpha)
                    );
                }

                // // create wall markings nr2 (100 divided by % number, 20% chance)
                // int varIndex = mazeDecoMap.try_emplace(gridPos, rand() % 5).first->second;
                // if (varIndex == 4) {
                //     srcFRect = { 32, 0, 32, 32 };
                //     renderQueue.push_back(
                //         RenderQueueItem(destTile.y + 1, srcFRect, destTile, &textureMap[Map::WALL_MARKINGS], alpha)
                //     );
                // }

                // // vines
                // // vaata grid_below, sest Vine tekstuur on vaid NS orientatsiooniga
                // int grid_below = map[row - 1][column];
                // if (rand() % 4 == 1 && !gridVineChecked.count(gridPos)) {
                //     // add overhang vines VINE_OVERHANG_SN
                //     if (grid_below == Map::MAZE_GROUND_CUBE &&
                //         (map[row - 2][column] == Map::SECTOR_2_WALL_VAL)) {
                //         map[row - 1][column] = Map::VINE_OVERHANG_SN;
                //     }
                //     // add overhang vines VINE_OVERHANG_EW
                //     if (map[row][column + 1] == Map::MAZE_GROUND_CUBE &&
                //         (map[row][column + 2] == Map::SECTOR_2_WALL_VAL)) {
                //         map[row][column + 1] = Map::VINE_OVERHANG_EW;
                //     }
                // }
                // if (rand() % 10 == 1 && !gridVineChecked.count(gridPos)) {
                //     // add VINE_COVER_N to wall's southern side
                //     if (grid_below == Map::MAZE_GROUND_CUBE ||
                //         grid_below == Map::VINE_OVERHANG_SN ||
                //         grid_below == Map::VINE_OVERHANG_EW) {
                //         map[row - 1][column] = Map::VINE_COVER_N;
                //         // expand vine to neighbouring blocks aswell.
                //         // FIXME: siin on bug. Kui vine on 6hus ss see ie ole feature. hetkel mitte.
                //         // Should be recursion but maze direction doesn't allow longer than 3 walls in sector2
                //         if (map[row][column + 1] == Map::VINE_OVERHANG_SN ||
                //             map[row][column + 1] == Map::VINE_OVERHANG_EW) {
                //             map[row - 1][column + 1] = Map::VINE_COVER_N;
                //         }
                //         if (map[row][column - 1] == Map::VINE_OVERHANG_SN ||
                //             map[row][column - 1] == Map::VINE_OVERHANG_EW) {
                //             map[row - 1][column - 1] = Map::VINE_COVER_N;
                //         }
                //     }
                // }
                // gridVineChecked.insert(gridPos);
                // // vine'il y += 1, et vine tekstuur oleks on top of wall
                // auto cubeVineTex = choose_cubeVineTexture("", gridPos);
                // if (cubeVineTex == nullptr) break;
                // renderQueue.push_back(
                //     RenderQueueItem(destTile.y + 2, destTile, cubeVineTex, alpha)
                // );
                break;
            }
            case Map::SECTOR_3_WALL_VAL: {
                if (unchangableWalls_S3.find(gridPos) == unchangableWalls_S3.end()) return;
                // create walls
                destTile.y -= halfTile;
                srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                if (!isEmpty(srcFRect)) {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y, srcFRect, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                    );
                }
                else {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y, srcFRect, destTile, &textureMap[Map::WALL_CUBE], alpha)
                    );
                }
                // create decoration
                // srcFRect = return_src_1x3(gridPos, mazeDecoMap);
                // if (!isEmpty(srcFRect)) renderQueue.push_back(
                //     RenderQueueItem(destTile.y + 1, srcFRect, destTile, &textureMap[Map::WALL_MARKINGS], alpha)
                // );
                // destTile.y -= halfTile;
                // srcFRect = return_src_1x3(gridPos, mazeDecoMap);
                // if (isEmpty(srcFRect)) {
                //     srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                //     if (!isEmpty(srcFRect)) renderQueue.push_back(
                //         RenderQueueItem(destTile.y + halfTile + 1, srcFRect, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                //     );
                //     break;
                // }
                // else {
                //     renderQueue.push_back(
                //         RenderQueueItem(destTile.y + halfTile + 1, srcFRect, destTile, &textureMap[Map::MAZE_DECO], alpha)
                //     );
                //     break;
                // }
                break;
            }
            }
        }
    }
}
void TerrainClass::render_decoration(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row < mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column < mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect;

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };

            switch (gridValue) {
            case Map::VINE_COVER_N:
            case Map::VINE_OVERHANG_SN:
            case Map::VINE_OVERHANG_EW: {
                textureMap[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
                destTile.y -= halfTile;
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, destTile, &textureMap[gridValue])
                );
                break;
            }
            default:
                textureMap[Map::ERROR_CUBE].render(renderer, &destTile);
                break;
            }
        }
    }
}
void TerrainClass::render_colored_cubes(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row < mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column < mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect;

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };

            switch (gridValue) {
            case Map::YELLOW_CUBE:
            case Map::ERROR_CUBE:
            case Map::BLUE_CUBE: {
                textureMap[gridValue].render(renderer, &destTile);
                break;
            }
            default:
                textureMap[Map::ERROR_CUBE].render(renderer, &destTile);
                break;
            }
        }
    }
}
void TerrainClass::render_items(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row < mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column < mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect;

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };

            switch (gridValue) {
            case Map::TREE_TRUNK: {
                destTile.y -= halfTile;
                int xr = randomOffsetsTrees.try_emplace(gridPos, rand() % 20)
                    .first->second;
                SDL_FRect tempTile = destTile;
                tempTile.x += (tileSize / 5) + xr;
                tempTile.y += (tileSize / 3) + xr;
                tempTile.h = (tileSize / 2) - xr;
                tempTile.w = (tileSize / 2) - xr;
                textureMap[gridValue].render(renderer, &tempTile);
                // create grass cover
                srcFRect = return_src_1x3(gridPos, grassCoverMap);
                if (!isEmpty(srcFRect)) textureMap[Map::GRASS_COVER_SPRITE].render(renderer, &srcFRect, &destTile);
                // int grassIndex = create_random_grass(gridPos, gridValue);
                // if (grassIndex != -1) textureMap[grassIndex].render(renderer, &destTile);
                break;
            }
            case Map::TREE: {
                destTile.y -= halfTile;
                int xr = randomOffsetsTrees.try_emplace(gridPos, rand() % 125)
                    .first->second;
                SDL_FRect tempTile = destTile;
                tempTile.y += (xr / 5);
                tempTile.x += (xr / 8);
                tempTile.w -= (xr / 4);
                tempTile.h -= (xr / 4);
                renderQueue.push_back(
                    RenderQueueItem(tempTile.y, tempTile, &textureMap[gridValue], alpha)
                );
                srcFRect = return_src_1x3(gridPos, grassCoverMap);
                if (!isEmpty(srcFRect)) textureMap[Map::GRASS_COVER_SPRITE].render(renderer, &srcFRect, &destTile);
                // int grassIndex = create_random_grass(gridPos, gridValue);
                // if (grassIndex != -1) textureMap[grassIndex].render(renderer, &destTile);
                break;
            }
            default:
                textureMap[Map::ERROR_CUBE].render(renderer, &destTile);
                break;
            }
        }
    }
}
void TerrainClass::render_renderQ(SDL_Renderer* renderer) {
    renderQueue.push_back(RenderQueueItem(static_cast<int>(player.rect.y - (tileSize / 2)), [](SDL_Renderer* renderer) { animation_player(renderer); }));
    std::sort(renderQueue.begin(), renderQueue.end(), [](const RenderQueueItem& a, const RenderQueueItem& b) { return a.renderOrder < b.renderOrder; });

    for (auto& r : renderQueue) {
        r.call_render(renderer);
    }
    renderQueue.clear();
}
void TerrainClass::render(SDL_Renderer* renderer) {
    calculate_miscellaneous();
    render_ground(renderer);
    // render_items(renderer);
    render_walls();
    // render_colored_cubes(renderer);
    // render_decoration(renderer);
    render_renderQ(renderer);
}