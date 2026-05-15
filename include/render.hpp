#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <utility>
#include <set>

#include "offset.hpp"
#include "game.hpp"
#include "textures.hpp"
#include "terrain.hpp"

extern std::unordered_map<uint32_t, int> randomOffsetsWalls;
extern std::unordered_map<uint32_t, int> randomOffsetsTrees;
extern std::unordered_map<uint32_t, int> randomOffsetsGround;
extern std::set<std::pair<int, int>> gridVineChecked;
extern std::unordered_map<uint32_t, int> decorationIndexMap;
extern std::unordered_set<uint32_t> sector3Cutouts;

// extern std::unordered_map<uint32_t, int> vineDecorationIndexMap;


// spritesheet indexes
namespace ssi {
    // wrapper if maxCol >= sizeOfSpritesheet, warp to next row.
    const int maxCols = 11;
    const int maxRows = 13;
    constexpr int maxSideLength = std::max(maxCols, maxRows);

    struct SpritesheetConfig {
        const int row;
        const int minColumn;
        const int maxColumn;
    };

    constexpr SpritesheetConfig wall{ 5, 0, 5 };
    constexpr SpritesheetConfig wallCubes{ 11, 0, 3 };
    constexpr SpritesheetConfig mazePathway{ 11, 7, 9 };
    constexpr SpritesheetConfig wallMarkings{ 12, 6, 8 };
    constexpr SpritesheetConfig ground{ 2, 0, 10 };
    // constexpr SpritesheetConfig groundDecoration{3, 8, 14}; //  needs wrap shit
    constexpr SpritesheetConfig groundDecoration{ 4, 0, 8 }; //  needs wrap shit
    constexpr SpritesheetConfig mazeGround{ 0, 0, 10 };
    constexpr SpritesheetConfig sec2Ground{ 1, 0, 5 };
    constexpr SpritesheetConfig sec3Ground{ 1, 0, 10 };
    constexpr SpritesheetConfig coverGround{ 12, 0, 2 };
    constexpr SpritesheetConfig coverMaze{ 12, 0, 5 };
    constexpr SpritesheetConfig empty{ 0, 0, 0 };
}

int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, int row, int minCol, int maxCol);
int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, const ssi::SpritesheetConfig& config);

// TerrainClass is defined in include/terrain.hpp
