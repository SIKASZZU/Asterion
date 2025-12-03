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
#include <array>
#include <unordered_set>
#include <utility>
#include <set>
#include <cstdint>


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

std::unordered_map<uint32_t, int> randomOffsetsWalls;
std::unordered_map<uint32_t, int> randomOffsetsTrees;
std::unordered_map<uint32_t, int> grassCoverGrids;
std::unordered_map<uint32_t, int> mazeDecoMap;
std::unordered_map<uint32_t, int> mazeGroundMap;
std::unordered_map<uint32_t, int> groundMap;
std::unordered_map<uint32_t, int> grassCoverMap;

// Per-tile, per-row random spritesheet frame index to avoid cross-category leakage.
static std::unordered_map<uint32_t, std::array<int, 11>> spritesheetIndexMap;
// Cache of source rects for spritesheet frames (keyed by (row<<8)|index)
static std::unordered_map<int, SDL_FRect> spritesheetCacheMap;

std::set<std::pair<int, int>> gridVineChecked;
std::vector<RenderQueueItem> renderQueue;

/// @brief
// Return cached src rect for spritesheet col and row (both zero-based). Initializes cache lazily.
/// @param col Image indexes from left to right NOT IMPLEMENTED! If col > spritesheet col then wraps around to the next row.
/// @param row Image indexes from top to bottom
/// @return Returns caches/gets-from-cache SDL_FRect for the col, row paramaters.
static const SDL_FRect& get_cached_spritesheet_src(int col, int row) {
    int c = std::max(0, std::min(col, 10));
    int r = std::max(0, std::min(row, 10));
    int key = (r << 8) | c;  // Unique key: row in high bits, col in low bits
    auto it = spritesheetCacheMap.find(key);
    if (it != spritesheetCacheMap.end()) return it->second;
    auto [insertedIt, _] = spritesheetCacheMap.emplace(key, SDL_FRect{
        static_cast<float>(c * Texture::spriteWidth),
        static_cast<float>(r * Texture::spriteHeight),
        static_cast<float>(Texture::spriteWidth),
        static_cast<float>(Texture::spriteHeight)
    });
    return insertedIt->second;
}

// spritesheet indexes
namespace ssi {
    // wrapper if maxCol >= sizeOfSpritesheet, warp to next row.
    const int sizeOfSpritesheet = 10;
    
    struct SpritesheetConfig {
        const int row;
        const int minColumn;
        const int maxColumn;
    };
    
    // Use constexpr for compile-time constants if possible (C++11+).
    constexpr SpritesheetConfig wall{5, 0, 5};
    constexpr SpritesheetConfig ground{2, 0, 10};
    constexpr SpritesheetConfig mazeGround{0, 0, 10};
}

// Return cached src rect for spritesheet col and row (both zero-based). Initializes cache lazily.
/// @brief
// Return cached src rect for spritesheet col and row (both zero-based). Initializes cache lazily.
/// @param gridPos 
/// @param row 
/// @param minCol From what index of col on the spritesheet the search has to start
/// @param maxCol From what index of col on the spritesheet the search has to end, NOT IMPLEMENTED, wraps around to new row if bigger then spritesheet col size.
/// @return Return the integar of index for that col and row position
static int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, int row, int minCol, int maxCol) {
    auto [it, inserted] = spritesheetIndexMap.try_emplace(make_grid_key(gridPos.first, gridPos.second), std::array<int, 11>{});
    if (inserted) it->second.fill(-1);
    int r = std::max(0, std::min(row, 10));
    int& stored = it->second[r];
    if (stored != -1) return stored;
    int range = std::max(1, maxCol - minCol + 1);
    stored = (rand() % range) + minCol;  // Uniform [minIndex..maxCol]
    return stored;
}

// Overload with const SpritesheetConfig& config instead
static int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, const ssi::SpritesheetConfig& config) {
    return ensure_spritesheet_index_for_row(gridPos, config.row, config.minColumn, config.maxColumn);
}


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
    bool inFront = ((row > playerTileY - 1 && row <= playerTileY + maxDist) && (column > playerTileX - 1 && column <= playerTileX + maxDist));
    if (inFront) { return inFrontAlpha; }
    return alpha;
}
int TerrainClass::create_random_grass(std::pair<int, int> gridPos, int gridValue) {
    int varIndex = grassCoverGrids.try_emplace(make_grid_key(gridPos.first, gridPos.second), rand() % 4)
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
SDL_FRect TerrainClass::return_src_1x3(std::pair<int, int> gridPos, std::unordered_map<uint32_t, int>& map) {
    int varIndex = map.try_emplace(make_grid_key(gridPos.first, gridPos.second), rand() % 4)
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
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };
            
            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            if (groundValues.find(gridValue) != groundValues.end()) {
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::ground);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::ground.row);
                destTile.y -= 16;
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);

            }
            if (wallValues.find(gridValue) != wallValues.end()
                || gridValue == Map::VINE_OVERHANG_SN 
                || gridValue == Map::VINE_OVERHANG_EW) {
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::mazeGround);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::mazeGround.row);
                destTile.y -= 16;
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);
            }
            switch (gridValue) {
            case Map::MAZE_GROUND_CUBE: {
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::mazeGround);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::mazeGround.row);
                destTile.y -= 16;
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);
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
    // Precompute which wall tiles should be shifted upwards when fading.
    std::unordered_set<uint32_t> shiftedWalls;
    for (int prow = mapIndexTop; prow <= mapIndexBottom; ++prow) {
        for (int pcol = mapIndexLeft; pcol <= mapIndexRight; ++pcol) {
            int pval = map[prow][pcol];
            if (wallValues.find(pval) == wallValues.end()) continue;
            if (determine_alpha({ prow, pcol }) == 1.0f) continue;

            // check vertical run (up and down) whether a pathway exists beyond the run
            bool foundPathVert = false;
            std::vector<std::pair<int, int>> vertRun;
            // expand up
            for (int r = prow; r >= mapIndexTop; --r) {
                if (map[r][pcol] == Map::MAZE_GROUND_CUBE) { foundPathVert = true; break; }
                if (wallValues.find(map[r][pcol]) == wallValues.end()) break;
                vertRun.emplace_back(r, pcol);
            }
            // expand down
            for (int r = prow + 1; r <= mapIndexBottom; ++r) {
                if (map[r][pcol] == Map::MAZE_GROUND_CUBE) { foundPathVert = true; break; }
                if (wallValues.find(map[r][pcol]) == wallValues.end()) break;
                vertRun.emplace_back(r, pcol);
            }
            if (foundPathVert) {
                for (auto& p : vertRun) shiftedWalls.insert(make_grid_key(p.first, p.second));
            }

            // check horizontal run (left and right)
            bool foundPathHorz = false;
            std::vector<std::pair<int, int>> horzRun;
            // expand left
            for (int c = pcol; c >= mapIndexLeft; --c) {
                if (map[prow][c] == Map::MAZE_GROUND_CUBE) { foundPathHorz = true; break; }
                if (wallValues.find(map[prow][c]) == wallValues.end()) break;
                horzRun.emplace_back(prow, c);
            }
            // expand right
            for (int c = pcol + 1; c <= mapIndexRight; ++c) {
                if (map[prow][c] == Map::MAZE_GROUND_CUBE) { foundPathHorz = true; break; }
                if (wallValues.find(map[prow][c]) == wallValues.end()) break;
                horzRun.emplace_back(prow, c);
            }
            if (foundPathHorz) {
                for (auto& p : horzRun) shiftedWalls.insert(make_grid_key(p.first, p.second));
            }
        }
    }

    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };
            float alpha = determine_alpha(gridPos);
            if (is_grid_not_renderable(gridPos)) continue;
            switch (gridValue) {
            case Map::INGROWN_WALL_CUBE: {
                destTile.y -= halfTile;
                destTile.y -= 5;
                destTile.x -= 5;
                destTile.w += 5;
                destTile.h += 5;
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, destTile, &textureMap[gridValue], alpha)
                );
                destTile.y -= halfTile;
                srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                if (!isEmpty(srcFRect)) {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y + halfTile + 1, srcFRect, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                    );
                }
                else {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y + halfTile + 1, destTile, &textureMap[Map::WALL_CUBE], alpha)
                    );
                }
                break;
            }
            case Map::WALL_CUBE: {
                destTile.y -= halfTile;
                if (shiftedWalls.find(make_grid_key(row, column)) != shiftedWalls.end()) {
                    alpha = inFrontAlpha;
                }
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
                break;
            }
            case Map::SECTOR_1_WALL_VAL: {
                destTile.y -= halfTile;

                int idx = ensure_spritesheet_index_for_row(gridPos, 5, 0, 5);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, 5);

                if (shiftedWalls.find(make_grid_key(row, column)) != shiftedWalls.end()) { alpha = inFrontAlpha; }
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, src, destTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                break;
            }
            case Map::SECTOR_2_WALL_VAL: {
                // create walls
                destTile.y -= halfTile;
                int xr = randomOffsetsWalls.try_emplace(make_grid_key(row, column), rand() % 30)
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
                // create random 2nd story walls based on decoration.
                destTile.y -= halfTile;
                srcFRect = return_src_1x3(gridPos, mazeDecoMap);
                if (isEmpty(srcFRect)) {
                    srcFRect = return_src_1x3(gridPos, mazeGroundMap);
                    if (!isEmpty(srcFRect)) renderQueue.push_back(
                        RenderQueueItem(destTile.y + halfTile + 1, srcFRect, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                    );
                }
                break;
            }
            }
        }
    }
}
void TerrainClass::render_decoration(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            destTile.y -= halfTile;

            switch (gridValue) {
            case Map::VINE_COVER_N:
            case Map::VINE_OVERHANG_SN:
            case Map::VINE_OVERHANG_EW: {
                textureMap[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, destTile, &textureMap[gridValue])
                );
                break;
            }
            case Map::MAZE_GROUND_CUBE: {
                srcFRect = return_src_1x3(gridPos, mazeDecoMap);
                if (isEmpty(srcFRect)) break;
                textureMap[Map::MAZE_DECO].render(renderer, &srcFRect, &destTile);
                break;
            }
            case Map::GROUND_CUBE: {
                srcFRect = return_src_1x3(gridPos, grassCoverMap);
                if (!isEmpty(srcFRect)) textureMap[Map::GRASS_COVER_SPRITE].render(renderer, &srcFRect, &destTile);
                break;
            }
            case Map::SECTOR_1_WALL_VAL:
            case Map::SECTOR_2_WALL_VAL: {
                break;

                if (gridValue == Map::SECTOR_2_WALL_VAL) { srcFRect = { 32, 0, 32, 32 }; }
                else { srcFRect = { 0, 0, 32, 32 }; };
                // create wall markings nr2 (100 divided by % number, 20% chance)
                int varIndex = mazeDecoMap.try_emplace(make_grid_key(row, column), rand() % 5).first->second;
                if (varIndex == 4) {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y + 1, srcFRect, destTile, &textureMap[Map::WALL_MARKINGS], alpha)
                    );
                }
                if (gridValue != Map::SECTOR_2_WALL_VAL) break;
                // vaata grid_below, sest Vine tekstuur on vaid NS orientatsiooniga
                // create hanging vines from wall to wall
                int grid_below = map[row - 1][column];
                if (rand() % 4 == 1 && !gridVineChecked.count(gridPos)) {
                    // add overhang vines VINE_OVERHANG_SN
                    if (grid_below == Map::MAZE_GROUND_CUBE &&
                        (map[row - 2][column] == Map::SECTOR_2_WALL_VAL)) {
                        map[row - 1][column] = Map::VINE_OVERHANG_SN;
                    }
                    // add overhang vines VINE_OVERHANG_EW
                    if (map[row][column + 1] == Map::MAZE_GROUND_CUBE &&
                        (map[row][column + 2] == Map::SECTOR_2_WALL_VAL)) {
                        map[row][column + 1] = Map::VINE_OVERHANG_EW;
                    }
                }
                // create vines on the sides of walls
                if (rand() % 10 == 1 && !gridVineChecked.count(gridPos)) {
                    // add VINE_COVER_N to wall's southern side
                    if (grid_below == Map::MAZE_GROUND_CUBE ||
                        grid_below == Map::VINE_OVERHANG_SN ||
                        grid_below == Map::VINE_OVERHANG_EW) {
                        map[row - 1][column] = Map::VINE_COVER_N;
                        // expand vine to neighbouring blocks aswell.
                        // FIXME: siin on bug. Kui vine on 6hus ss see ie ole feature. hetkel mitte.
                        // Should be recursion but maze direction doesn't allow longer than 3 walls in sector2
                        if (map[row][column + 1] == Map::VINE_OVERHANG_SN ||
                            map[row][column + 1] == Map::VINE_OVERHANG_EW) {
                            map[row - 1][column + 1] = Map::VINE_COVER_N;
                        }
                        if (map[row][column - 1] == Map::VINE_OVERHANG_SN ||
                            map[row][column - 1] == Map::VINE_OVERHANG_EW) {
                            map[row - 1][column - 1] = Map::VINE_COVER_N;
                        }
                    }
                }
                gridVineChecked.insert(gridPos);
                auto cubeVineTex = choose_cube_vine_texture("", gridPos);
                if (cubeVineTex != nullptr) {
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y + halfTile + 2, destTile, cubeVineTex, alpha)
                    );
                }
                break;
            }
            case Map::SECTOR_3_WALL_VAL: {
                // create decoration
                srcFRect = return_src_1x3(gridPos, mazeDecoMap);
                if (!isEmpty(srcFRect)) renderQueue.push_back(
                    RenderQueueItem(destTile.y + 1, srcFRect, destTile, &textureMap[Map::WALL_MARKINGS], alpha)
                );
                srcFRect = return_src_1x3(gridPos, mazeDecoMap);

                if (!isEmpty(srcFRect)) {
                    destTile.y -= halfTile;
                    renderQueue.push_back(
                        RenderQueueItem(destTile.y + halfTile + 1, srcFRect, destTile, &textureMap[Map::MAZE_DECO], alpha)
                    );
                };
                break;
            }
            case Map::TREE:
            case Map::TREE_TRUNK: {
                // create grass cover
                srcFRect = return_src_1x3(gridPos, grassCoverMap);
                if (!isEmpty(srcFRect)) textureMap[Map::GRASS_COVER_SPRITE].render(renderer, &srcFRect, &destTile);
                // int grassIndex = create_random_grass(gridPos, gridValue);
                // if (grassIndex != -1) textureMap[grassIndex].render(renderer, &destTile);
            }
            }
        }
    }
}
void TerrainClass::render_colored_cubes(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            switch (gridValue) {
            case Map::YELLOW_CUBE:
            case Map::ERROR_CUBE:
            case Map::BLUE_CUBE: {
                textureMap[gridValue].render(renderer, &destTile);
                break;
            }
            }
        }
    }
}
void TerrainClass::render_items(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };

            int gridValue = map[row][column];
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            switch (gridValue) {
            case Map::TREE_TRUNK: {
                destTile.y -= halfTile;
                int xr = randomOffsetsTrees.try_emplace(make_grid_key(row, column), rand() % 20)
                    .first->second;
                SDL_FRect tempTile = destTile;
                tempTile.x += (tileSize / 5) + xr;
                tempTile.y += (tileSize / 3) + xr;
                tempTile.h = (tileSize / 2) - xr;
                tempTile.w = (tileSize / 2) - xr;
                textureMap[gridValue].render(renderer, &tempTile);
                break;
            }
            case Map::TREE: {
                destTile.y -= halfTile;
                int xr = randomOffsetsTrees.try_emplace(make_grid_key(row, column), rand() % 125)
                    .first->second;
                SDL_FRect tempTile = destTile;
                tempTile.y += (xr / 5);
                tempTile.x += (xr / 8);
                tempTile.w -= (xr / 4);
                tempTile.h -= (xr / 4);
                renderQueue.push_back(
                    RenderQueueItem(tempTile.y, tempTile, &textureMap[gridValue], alpha)
                );
                break;
            }
            }
        }
    }
}
void TerrainClass::render_entities() {
    // player
    renderQueue.push_back(RenderQueueItem(static_cast<int>(player.rect.y - (tileSize / 2)), [](SDL_Renderer* renderer) { animation_player(renderer); }));
    // add enemies to renderQ

    for (auto& e : enemyArray) {
        int yPos = static_cast<int>(e.get_rect().y - tileSize / 2);
        renderQueue.push_back(RenderQueueItem(yPos, [&e](SDL_Renderer* renderer) { e.render(renderer); }));
    }
}
void TerrainClass::render_renderQ(SDL_Renderer* renderer) {
    std::sort(renderQueue.begin(), renderQueue.end(), [](const RenderQueueItem& a, const RenderQueueItem& b) { return a.renderOrder < b.renderOrder; });
    for (auto& r : renderQueue) { r.call_render(renderer); }
    renderQueue.clear();
}
void TerrainClass::render(SDL_Renderer* renderer) {
    calculate_miscellaneous();

    render_ground(renderer);
    render_items(renderer);
    render_walls();
    render_colored_cubes(renderer);
    // render_decoration(renderer);
    render_entities();
    render_renderQ(renderer);
}
