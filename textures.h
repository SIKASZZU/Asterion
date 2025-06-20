#include <iostream>
#include <SDL2/SDL.h>

extern SDL_Texture* ground_tex;
extern SDL_Texture* tree_tex;
extern SDL_Texture* numbers_tex;

extern const int digit_width;
extern const int digit_height;
extern const int numbers_tex_y;

void load_textures(SDL_Renderer* renderer);
void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect at_tile);