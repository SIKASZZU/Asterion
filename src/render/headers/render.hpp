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

extern std::set<std::pair<int, int>> gridVineChecked;
extern std::unordered_map<uint32_t, int> randomOffsetsWalls;
extern std::unordered_map<uint32_t, int> randomOffsetsTrees;
extern std::unordered_map<uint32_t, int> grassCoverGrids;
extern std::unordered_map<uint32_t, int> mazeDecoMap;
extern std::unordered_map<uint32_t, int> mazeGroundMap;

extern std::unordered_map<uint32_t, int> groundMap;
extern std::unordered_map<uint32_t, int> grassCoverMap;


struct RenderQueueItem {
    int renderOrder;
    SDL_FRect dstrect;
    SDL_FRect srcrect = { 0,0,0,0 };
    // if this is defined then texture method for rendering will be used
    std::optional<Texture*> texture;
    // function to call when rendering needs to be done, must contain 
    // everything it needs, except the renderer
    // try not to use this as thousands of lambda functions 
    // will cause a performance hit
    float alpha = 1.0f;

    std::optional<std::function<void(SDL_Renderer* renderer)>> customRender;
    RenderQueueItem(int renderOrder, SDL_FRect dstrect, Texture* texture, float alpha);
    RenderQueueItem(int renderOrder, SDL_FRect srcrect, SDL_FRect dstrect, Texture* texture, float alpha);
    RenderQueueItem(int renderOrder, std::function<void(SDL_Renderer* renderer)> customRender);
    void call_render(SDL_Renderer* renderer);
};

extern std::vector<RenderQueueItem> renderQueue;

class TerrainClass {
private:
    SDL_FRect return_destTile(int row, int column);
    bool is_grid_not_renderable(std::pair<int, int> gridPos);
    float determine_alpha(std::pair<int, int> gridPos);
    int create_random_grass(std::pair<int, int> gridPos, int gridValue);
    void render_ground(SDL_Renderer* renderer);
    void render_walls();
    void render_decoration(SDL_Renderer* renderer);
    void render_items(SDL_Renderer* renderer);
    void render_renderQ(SDL_Renderer* renderer);
    void render_colored_cubes(SDL_Renderer* renderer);
    void render_entities();
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
public:
    void calculate_miscellaneous(float dT);
    void render(SDL_Renderer* renderer);
};
