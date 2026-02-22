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

extern std::unordered_map<uint32_t, int> randomOffsetsWalls;
extern std::unordered_map<uint32_t, int> randomOffsetsTrees;
extern std::set<std::pair<int, int>> gridVineChecked;
extern std::unordered_map<uint32_t, int> decorationIndexMap;
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
    constexpr SpritesheetConfig coverGround{ 4, 0, 2 };
    constexpr SpritesheetConfig coverMaze{ 12, 3, 5 };
    constexpr SpritesheetConfig empty{ 0, 0, 0 };
}

int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, int row, int minCol, int maxCol);
int ensure_spritesheet_index_for_row(std::pair<int, int> gridPos, const ssi::SpritesheetConfig& config);

class TerrainClass {
private:
    SDL_FRect return_destTile(int row, int column);
    bool is_grid_not_renderable(std::pair<int, int> gridPos);
    float determine_alpha(std::pair<int, int> gridPos);
    void create_renderQ_ground(SDL_Renderer* renderer);
    void create_renderQ_walls();
    void create_renderQ_decoration(SDL_Renderer* renderer);
    void create_renderQ_items(SDL_Renderer* renderer);
    void create_renderQ_colored_cubes(SDL_Renderer* renderer);
    void create_renderQ_entities();
    void render_entity_grid_highlights(SDL_Renderer* renderer);
    void render_renderQ(SDL_Renderer* renderer);
    int mapIndexLeft;
    int mapIndexRight;
    int mapIndexTop;
    int mapIndexBottom;
    int playerTileX;
    int playerTileY;
    int halfTile;
    // TODO: igal render catil oleks eraldi renderQ, et wallid v6istlevad oma enda vahel jne.
    // render q's ei ole groundi pildid ehk id 1, 4, 5. V6ib lisanduda!
    const float alpha = 1.0f;
    const float inFrontAlpha = alpha * 0.6f;
    float savedDoorY = 0;
    float elapsedDistance = 0;
    const float doorMovingSpeed = 2.0f;
    const int imageShift = 24;
public:
    void calculate_miscellaneous(float dT);
    void render(SDL_Renderer* renderer);
};
