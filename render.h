#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "game.h"
#include "textures.h"

struct RenderQueueItem {
  int z_index;
  SDL_Rect dstrect;
  // if this is defined then texture method for rendering will be used
  std::optional<Texture*> texture;
  // function to call when rendering needs to be done, must contain 
  // everything it needs, except the renderer
  // try not to use this as thousands of lambda functions 
  // will cause a performance hit
  std::optional<std::function<void(SDL_Renderer* renderer)>> custom_render;

  RenderQueueItem(int z_index, SDL_Rect dstrect, Texture* texture);
  RenderQueueItem(int z_index, std::function<void(SDL_Renderer* renderer)> custom_render);

  void render(SDL_Renderer* renderer);
};


void load_render(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

void render_map(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

void render_map_numbers(SDL_Renderer* renderer, struct Offset& offset, struct Player& player);

extern std::unordered_map<std::pair<int, int>, int, pair_hash> random_offsets;

