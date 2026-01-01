#include "textures.hpp"
#include "collision.hpp"
#include "render.hpp"
#include "isometric_calc.hpp"
#include "raycast.hpp"
#include "mod_map_data.hpp"
#include "animation.hpp"
#include "daylight.hpp"
#include "minimap.hpp"
#include "raycast.hpp"
#include "renderQueue.hpp"

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <array>
#include <unordered_set>
#include <utility>
#include <set>
#include <cstdint>

std::set<std::pair<int, int>> gridVineChecked;
std::unordered_map<uint32_t, int> randomOffsetsWalls;
std::unordered_map<uint32_t, int> randomOffsetsTrees;
std::unordered_map<uint32_t, int> randomOffsetsGround;
// key, index of decoration. You need to know key to SpritesheetConfig. Else wrong texture, jsut random index of spritesheet.
std::unordered_map<uint32_t, int> decorationIndexMap;
// std::unordered_map<uint32_t, int> vineDecorationIndexMap;
// Per-tile, per-row random spritesheet frame index to avoid cross-category leakage.
static std::unordered_map<uint32_t, std::array<int, ssi::maxSideLength>> spritesheetIndexMap;
// Cache of source rects for spritesheet frames (keyed by (row<<8)|index)
static std::unordered_map<int, SDL_FRect> spritesheetCacheMap;

/// @brief
/// Return cached src rect for spritesheet col and row (both zero-based). Initializes cache lazily.
/// If doesn't exist, creates a src based on pos.
/// @param col Image indexes from left to right NOT IMPLEMENTED! If col > spritesheet col then wraps around to the next row.
/// @param row Image indexes from top to bottom
/// @return Returns caches/gets-from-cache SDL_FRect for the col, row paramaters.
static const SDL_FRect& get_cached_spritesheet_src(int col, int row) {
    int c = std::max(0, std::min(col, ssi::maxCols));
    int r = std::max(0, std::min(row, ssi::maxRows));
    uint32_t key = make_grid_key(r, c);
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

/// @brief
/// Return index of image of grid pos.
/// If doesn't exist, assigns that pos.
/// @param gridPos 
/// @param row 
/// @param minCol From what index of col on the spritesheet the search has to start
/// @param maxCol From what index of col on the spritesheet the search has to end, NOT IMPLEMENTED, wraps around to new row if bigger then spritesheet col size.
/// @return Return the integar of index for that col and row position
int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, int row, int minCol, int maxCol) {
    auto [it, inserted] = spritesheetIndexMap.try_emplace(make_grid_key(gridPos.first, gridPos.second), std::array<int, ssi::maxSideLength>{});
    if (inserted) it->second.fill(-1);
    int r = std::max(0, std::min(row, ssi::maxRows));
    int& stored = it->second[r];
    if (stored != -1) return stored;
    int range = std::max(1, maxCol - minCol + 1);
    stored = (rand() % range) + minCol;  // Uniform [minIndex..maxCol]
    return stored;
}

// Overload with const SpritesheetConfig& config instead
int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, const ssi::SpritesheetConfig& config) {
    return ensure_spritesheet_index_for_row(gridPos, config.row, config.minColumn, config.maxColumn);
}


SDL_FRect TerrainClass::return_destTile(int row, int column) {
    SDL_FPoint isometricCoordinates = to_isometric_grid_coordinate(column, row);
    return SDL_FRect{ isometricCoordinates.x, isometricCoordinates.y, tileSize, tileSize };
}
bool TerrainClass::is_grid_not_renderable(std::pair<int, int> gridPos) {
    if (Raycast::enabled == false) {}
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
void TerrainClass::calculate_miscellaneous(float dT) {
    elapsedDistance += doorMovingSpeed * dT;
    halfTile = tileSize / 2;
    renderQueue.reserve(4 * (renderRadius * renderRadius));
    playerTileX = static_cast<int>((player.x + player.size / 2) / tileSize);
    playerTileY = static_cast<int>((player.y + player.size / 2) / tileSize);

    mapIndexLeft = std::max(0, playerTileX - renderRadius);
    mapIndexTop = std::max(0, playerTileY - renderRadius);
    mapIndexRight = std::min(mapSize - 1, playerTileX + renderRadius);
    mapIndexBottom = std::min(mapSize - 1, playerTileY + renderRadius);

}
void TerrainClass::create_renderQ_ground(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };
            int gridValue = map[row][column];

            if (groundValues.find(gridValue) != groundValues.end()) {
                int xr = randomOffsetsGround.try_emplace(make_grid_key(row, column), rand() % groundMod)
                    .first->second;
                destTile.y += (xr);
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::ground);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::ground.row);
                destTile.y -= imageShift;
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);
            }
            if (wallValues.find(gridValue) != wallValues.end()
                || gridValue == Map::VINE_OVERHANG_SN
                || gridValue == Map::VINE_OVERHANG_EW) {
                int xr = randomOffsetsGround.try_emplace(make_grid_key(row, column), rand() % groundMod)
                    .first->second;
                xr = std::max(4, xr);
                destTile.y += (xr);
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::mazeGround);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::mazeGround.row);
                destTile.y -= imageShift;
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);
            }
            switch (gridValue) {
            case Map::VOID_CUBE:
            case Map::VOID_CUBE_NEIGHBOUR: {
                render_void_tilemap(renderer, offset, map, gridPos, destTile);
                break;
            }
            case Map::MAZE_WE_DOOR:
            case Map::MAZE_NS_DOOR:
            case Map::SECTOR_1_PATHWAY: {
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::mazeGround);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::mazeGround.row);
                destTile.y -= imageShift;
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);
                break;
            }
            case Map::MAZE_GROUND_CUBE:
            case Map::SECTOR_2_PATHWAY:
            case Map::SECTOR_3_PATHWAY: {
                int xr = randomOffsetsGround.try_emplace(make_grid_key(row, column), rand() % groundMod)
                    .first->second;
                xr = std::max(4, xr);
                destTile.y += (xr);
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::mazePathway);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::mazePathway.row);
                destTile.y -= imageShift;
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);
                break;
            }
            }
        }
    }
}
void TerrainClass::create_renderQ_walls() {
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
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;
            int gridValue = map[row][column];

            SDL_FRect destTile = return_destTile(row, column);
            uint32_t key = make_grid_key(row, column);
            float alpha = determine_alpha(gridPos);

            auto it = randomOffsetsGround.find(key);
            if (it != randomOffsetsGround.end()) {
                destTile.y += it->second;
            }

            switch (gridValue) {
            case Map::INGROWN_WALL_CUBE: {
                destTile.y -= halfTile;
                destTile.y -= 5;
                destTile.x -= 5;
                destTile.w += 5;
                destTile.h += 5;
                SDL_FRect src;
                src = get_cached_spritesheet_src(4, 11);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, src, destTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                destTile.y -= halfTile;
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::wallCubes);
                src = get_cached_spritesheet_src(idx, ssi::wallCubes.row);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y + halfTile + 1, src, destTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                break;
            }
            case Map::WALL_CUBE: {
                destTile.y -= halfTile;
                // if (shiftedWalls.find(key) != shiftedWalls.end()) {
                //     alpha = inFrontAlpha;
                // }
                shiftedWalls.find(key) == shiftedWalls.end() ? : alpha = inFrontAlpha;
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::wallCubes);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::wallCubes.row);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y + halfTile + 1, src, destTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                break;
            }
            case Map::SECTOR_1_WALL_VAL: {
                destTile.y -= halfTile;

                int idx = ensure_spritesheet_index_for_row(gridPos, 5, 0, 5);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, 5);
                // if (shiftedWalls.find(key) != shiftedWalls.end()) { alpha = inFrontAlpha; }
                shiftedWalls.find(key) == shiftedWalls.end() ? : alpha = inFrontAlpha;
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, src, destTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                break;
            }
            case Map::SECTOR_2_WALL_VAL: {
                // create walls
                destTile.y -= halfTile;
                int xr = randomOffsetsWalls.try_emplace(key, rand() % 30)
                    .first->second;
                destTile.y += (xr / 2.5);
                destTile.x += (xr / 4);
                destTile.w -= (xr / 2);
                destTile.h -= (xr / 2);
                int idx = ensure_spritesheet_index_for_row(gridPos, ssi::wallCubes);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::wallCubes.row);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, src, destTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                break;
            }
            case Map::SECTOR_3_WALL_VAL: {
                if (unchangableWalls_S3.find(gridPos) == unchangableWalls_S3.end()) return;
                // create walls
                destTile.y -= halfTile;
                SDL_FRect src;
                int idx = 0;
                idx = ensure_spritesheet_index_for_row(gridPos, ssi::wallCubes);
                src = get_cached_spritesheet_src(idx, ssi::wallCubes.row);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, src, destTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                // create random 2nd story walls based on decoration.
                destTile.y -= halfTile;
                idx = ensure_spritesheet_index_for_row(gridPos, ssi::wallCubes);
                src = get_cached_spritesheet_src(idx, ssi::wallCubes.row);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y + halfTile + 1, src, destTile, &textureMap[Map::WALL_CUBE_SPRITE], alpha)
                );
                break;
            }
            }
        }
    }
}
void TerrainClass::create_renderQ_decoration(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };
            int gridValue = map[row][column];
            uint32_t key = make_grid_key(row, column);

            destTile.y -= halfTile;

            auto it = randomOffsetsGround.find(key);
            if (it != randomOffsetsGround.end()) {
                destTile.y += it->second;
            }

            switch (gridValue) {
            case Map::VINE_COVER_N:
            case Map::VINE_OVERHANG_SN:
            case Map::VINE_OVERHANG_EW: {
                // textureMap[Map::MAZE_GROUND_CUBE].render(renderer, &destTile);
                // renderQueue.push_back(
                //     RenderQueueItem(destTile.y, destTile, &textureMap[gridValue])
                // );
                break;
            }
                                      // case Map::MAZE_GROUND_CUBE: {
                                      // case Map::SECTOR_2_PATHWAY:
            case Map::SECTOR_3_PATHWAY: {
                auto it = decorationIndexMap.find(key);
                if (it == decorationIndexMap.end()) break; // no decoration for this tile
                int idx = it->second;
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::coverMaze.row);
                textureMap[Map::SPRITESHEET].render(renderer, &src, &destTile);
                break;
            }
            case Map::GROUND_CUBE: {
                auto it = decorationIndexMap.find(key);
                if (it == decorationIndexMap.end()) break; // no decoration for this tile
                int idx = it->second;
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::groundDecoration.row);
                renderQueue.push_back(RenderQueueItem(destTile.y + 1, src, destTile, &textureMap[Map::SPRITESHEET], alpha));
                break;
            }
            case Map::SECTOR_3_WALL_VAL: {
                destTile.y -= halfTile;
                auto it = decorationIndexMap.find(key);
                if (it == decorationIndexMap.end()) break; // no decoration for this tile
                int idx = it->second;
                const SDL_FRect& src = get_cached_spritesheet_src(idx, ssi::coverMaze.row);
                renderQueue.push_back(RenderQueueItem(destTile.y + halfTile + 1, src, destTile, &textureMap[Map::SPRITESHEET], alpha));
                break;
            }
            }
        }
    }
}
void TerrainClass::create_renderQ_colored_cubes(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };
            int gridValue = map[row][column];

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
void TerrainClass::create_renderQ_items(SDL_Renderer* renderer) {
    for (int row = mapIndexTop; row <= mapIndexBottom; row++) {
        for (int column = mapIndexLeft; column <= mapIndexRight; column++) {
            std::pair<int, int> gridPos = { row, column };
            if (is_grid_not_renderable(gridPos)) continue;

            SDL_FRect destTile = return_destTile(row, column);
            SDL_FRect srcFRect = { 0, 0, 0, 0 };
            int gridValue = map[row][column];

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
            case Map::MAZE_WE_DOOR:
            case Map::MAZE_NS_DOOR: {
                SDL_FRect renderTile = destTile;
                float relativeTargetClosed = -halfTile;
                bool gladeDoorsClosed = daylightSettings.day;
                if (gladeDoorsClosed) {
                    relativeTargetClosed -= halfTile;
                }
                if (savedDoorY == 0) {
                    savedDoorY = relativeTargetClosed;
                }
                if (savedDoorY < relativeTargetClosed) {
                    savedDoorY = std::min(savedDoorY + elapsedDistance, relativeTargetClosed);
                }
                else if (savedDoorY > relativeTargetClosed) {
                    savedDoorY = std::max(savedDoorY - elapsedDistance, relativeTargetClosed);
                }
                elapsedDistance = 0;
                renderTile.y = destTile.y + savedDoorY;
                destTile.y -= halfTile;
                int row = 11;
                int minCol = 5; int maxCol = 5;
                if (gridValue == MAZE_WE_DOOR) {
                    minCol = 6; maxCol = 6;
                }
                int idx = ensure_spritesheet_index_for_row(gridPos, row, minCol, maxCol);
                const SDL_FRect& src = get_cached_spritesheet_src(idx, row);
                renderQueue.push_back(
                    RenderQueueItem(destTile.y, src, renderTile, &textureMap[Map::SPRITESHEET], alpha)
                );
                break;
            }
            }
        }
    }
}
void TerrainClass::create_renderQ_entities() {
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
    create_renderQ_ground(renderer);
    create_renderQ_items(renderer);
    create_renderQ_walls();
    create_renderQ_colored_cubes(renderer);
    create_renderQ_decoration(renderer);
    create_renderQ_entities();
    render_renderQ(renderer);
    // overlay minimap on top-right
    render_minimap(renderer);
}
