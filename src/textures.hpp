#pragma once
#include "game.hpp"
#include "offset.hpp"

#include <SDL3/SDL.h>
#include <vector>
#include <iostream>
#include <unordered_map>

class Texture {
protected:
  SDL_Texture* m_texture;
public:
  SDL_Texture* get_texture() { return m_texture; }

  void destroy_texture();
  void render(SDL_Renderer* renderer, const SDL_FRect* srcrect, const SDL_FRect* dstrect);
  void render(SDL_Renderer* renderer, const SDL_FRect* dstrect);
};

extern std::unordered_map<int, Texture> texture_map;
// igal id 8 gridil eraldi random vine
extern std::unordered_map<std::pair<int, int>, int, pair_hash> grid_vines;

void load_specific_number(SDL_Renderer* renderer, int number, SDL_FRect destTile);
void load_textures(SDL_Renderer* renderer);
void destroy_all_textures();
void render_void_tilemap(SDL_Renderer* renderer, struct Offset& offset,
int map[map_size][map_size], std::pair<int, int> grid_pos, SDL_FRect destTile);

Texture* choose_cube_vine_texture(std::string type = "", std::pair<int, int> grid_pos = { -1, -1 });

