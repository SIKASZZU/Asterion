#pragma once
#include <SDL2/SDL.h>
#include "game.hpp"

#include <vector>
#include <iostream>
#include <unordered_map>

class Texture {
protected:
  SDL_Texture* m_texture;
public:
  SDL_Texture* get_texture() { return m_texture; }

  void destroy_texture();
  void render(SDL_Renderer* renderer, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
  void render(SDL_Renderer* renderer, const SDL_Rect* dstrect);
};

extern std::unordered_map<int, Texture> texture_map;
// igal id 8 gridil eraldi random vine
extern std::unordered_map<std::pair<int, int>, int, pair_hash> grid_vines;
extern Uint32 last_update;  // Used for animation timing
extern int last_frame;      // Remember index of last player anim
extern int row;             // Used for accessing player anim texture
extern int player_animation_speed;


void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect destTile);
void load_textures(SDL_Renderer* renderer);
void destroy_all_textures();
void load_player_sprite(SDL_Renderer* renderer);

Texture* choose_cube_vine_texture(std::string type = "", std::pair<int, int> grid_pos = { -1, -1 });

