#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <utility>
#include <set>

#include "game.hpp"
#include "textures.hpp"

extern std::set<std::pair<int, int>> grid_vine_checked;
extern std::unordered_map<std::pair<int, int>, int, pair_hash> grassCoverGrids;
extern std::unordered_map<std::pair<int, int>, int, pair_hash> mazeDecoMap;
extern std::unordered_map<std::pair<int, int>, int, pair_hash> mazeGroundMap;

struct RenderQueueItem {
  int render_order;
  SDL_FRect dstrect;
  SDL_FRect srcrect = {0,0,0,0};
  // if this is defined then texture method for rendering will be used
  std::optional<Texture*> texture;
  // function to call when rendering needs to be done, must contain 
  // everything it needs, except the renderer
  // try not to use this as thousands of lambda functions 
  // will cause a performance hit
  std::optional<std::function<void(SDL_Renderer* renderer)>> custom_render;

  RenderQueueItem(int render_order, SDL_FRect dstrect, Texture* texture);
  RenderQueueItem(int render_order, SDL_FRect srcrect, SDL_FRect dstrect, Texture* texture);
  RenderQueueItem(int render_order, std::function<void(SDL_Renderer* renderer)> custom_render);

  void render(SDL_Renderer* renderer);
};


void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

void render_map_numbers(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

int create_random_grass(std::pair<int, int> grid_pos, int grid_value);
SDL_FRect return_src_1x3(std::pair<int, int> grid_pos, std::unordered_map<std::pair<int, int>, int, pair_hash>& map);

extern std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets_walls;
extern std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets_trees;