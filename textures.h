#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "game.h"

extern SDL_Texture* cube_error_tex;
extern SDL_Texture* ground_tex;
extern SDL_Texture* cube_ground_tex;
extern SDL_Texture* cube_snowy_ground_tex;
extern SDL_Texture* tree_tex;
extern SDL_Texture* snowy_tree_tex;
extern SDL_Texture* numbers_tex;
extern SDL_Texture* cube_wall_tex;
extern SDL_Texture* cube_vine_hard_tex;
extern SDL_Texture* cube_vine_medium_tex;
extern SDL_Texture* cube_vine_soft_tex;
extern SDL_Texture* cube_yellow_tex;
extern SDL_Texture* cube_blue_tex;
extern SDL_Texture* cube_maze_ground_tex;
extern SDL_Texture* cube_ingrown_wall_tex;

extern const int digit_width;
extern const int digit_height;

extern std::vector<SDL_Texture*> texture_vector;
extern std::unordered_map<std::pair<int, int>, int, pair_hash> grid_vines;  // igal id 8 gridil eraldi random vine

void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect destTile);
void load_textures(SDL_Renderer* renderer);
void load_cube_wall_texture(SDL_Renderer* renderer, SDL_Rect at_tile);
void load_cube_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile);
void load_cube_maze_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile);
void load_cube_snowy_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile);
void load_cube_error_texture(SDL_Renderer* renderer, SDL_Rect destTile);
void load_cube_yellow_texture(SDL_Renderer* renderer, SDL_Rect destTile);
void load_cube_blue_texture(SDL_Renderer* renderer, SDL_Rect destTile);
void destroy_all_textures();

SDL_Texture* choose_cube_vine_texture(std::string type = "", std::pair<int, int> grid_pos = { -1, -1 });
