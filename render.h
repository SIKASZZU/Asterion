#include "game.h"
#include <iostream>

void load_render(SDL_Renderer* renderer, const int tile_size, struct Offset& offset);

void render_map(SDL_Renderer* renderer, const int tile_size, struct Offset& offset,
                SDL_Texture* tree_tex, SDL_Texture* wall_tex, SDL_Texture* snowy_ground_tex);

void render_map_numbers(SDL_Renderer* renderer, const int tile_size, struct Offset& offset);
