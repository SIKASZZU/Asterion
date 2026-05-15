#pragma once

#include <SDL3/SDL.h>
#include <utility>
#include <string>
#include <set>
#include <unordered_map>

struct GameState;

namespace ssi {
    struct SpritesheetConfig;
}

class TerrainClass {
private:
    SDL_FRect return_destTile(int row, int column);
    bool is_grid_not_renderable(std::pair<int, int> gridPos, int gridValue);
    float determine_alpha(std::pair<int, int> gridPos);
    void create_renderQ_ground();
    void create_renderQ_walls();
    void create_renderQ_decoration(SDL_Renderer* renderer);
    void create_renderQ_items(SDL_Renderer* renderer);
    void create_renderQ_colored_cubes(SDL_Renderer* renderer);
    void create_renderQ_entities();
    void render_entity_grid_highlights(GameState* gS);
    void render_renderQ(SDL_Renderer* renderer);
    void render_renderQ_ground(SDL_Renderer* renderer);
    int mapIndexLeft;
    int mapIndexRight;
    int mapIndexTop;
    int mapIndexBottom;
    int playerTileX;
    int playerTileY;
    int halfTile;
    const float alpha = 1.0f;
    const float inFrontAlpha = alpha * 0.6f;
    float savedDoorY = 0;
    float elapsedDistance = 0;
    const float doorMovingSpeed = 2.0f;
    const int imageShift = 24;
public:
    void calculate_miscellaneous(float dT);
    void render(GameState* gS);
    void update(SDL_Renderer* renderer);
};
