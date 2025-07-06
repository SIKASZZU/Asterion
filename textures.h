#pragma once
#include <SDL2/SDL.h>
#include <vector>

extern SDL_Texture* ground_tex;
extern SDL_Texture* cube_ground_tex;
extern SDL_Texture* cube_snowy_ground_tex;
extern SDL_Texture* tree_tex;
extern SDL_Texture* snowy_tree_tex;
extern SDL_Texture* numbers_tex;
extern SDL_Texture* cube_wall_tex;

extern const int digit_width;
extern const int digit_height;
extern std::vector<SDL_Texture> texture_vector;

void load_specific_number(SDL_Renderer *renderer, int number, SDL_Rect destTile);
void load_textures(SDL_Renderer *renderer);
void load_cube_wall_texture(SDL_Renderer *renderer, SDL_Rect at_tile);
void load_cube_ground_texture(SDL_Renderer *renderer, SDL_Rect destTile);
void load_cube_snowy_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile);
void destroy_all_textures();