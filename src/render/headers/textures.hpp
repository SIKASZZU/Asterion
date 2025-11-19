#pragma once
#include "game.hpp"
#include "offset.hpp"

#include <SDL3/SDL.h>
#include <vector>
#include <iostream>
#include <unordered_map>

class Texture {
protected:
  SDL_Texture* mTexture;
public:
  static constexpr float spriteWidth = 32;
  static constexpr float spriteHeight = 32;
  SDL_Texture* get_texture() { return mTexture; }
  void destroy_texture();
  void render(SDL_Renderer* renderer, const SDL_FRect* srcrect, const SDL_FRect* dstrect);
  void render(SDL_Renderer* renderer, const SDL_FRect* dstrect);
};

extern std::unordered_map<int, Texture> textureMap;
// igal id 8 gridil eraldi random vine
extern std::unordered_map<std::pair<int, int>, int, pair_hash> gridVines;

void load_textures(SDL_Renderer* renderer);
void destroy_all_textures();
void render_void_tilemap(SDL_Renderer* renderer, struct Offset& offset,
  int map[mapSize][mapSize], std::pair<int, int> gridPos, SDL_FRect destTile);

Texture* choose_cube_vine_texture(std::string type = "", std::pair<int, int> gridPos = { -1, -1 });

