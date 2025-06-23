#include <iostream>
#include <SDL2/SDL.h>
#include <map>

extern SDL_Texture* ground_tex;
extern SDL_Texture* snowy_ground_tex;
extern SDL_Texture* tree_tex;
extern SDL_Texture* numbers_tex;
extern SDL_Texture* wall_tex;

extern const int digit_width;
extern const int digit_height;

extern std::string ground_tex_path;
extern std::map<std::pair<int, int>, std::string> row_columnToPath;
extern std::map<std::string, SDL_Texture*> texture_cache;

void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect destTile);
void draw_wall(SDL_Renderer* renderer, SDL_Texture* wall_tex, const bool neighbors[3][3], SDL_Rect destTile);

void load_textures(SDL_Renderer* renderer);
void load_wall_texture(SDL_Renderer* renderer, SDL_Texture* wall_tex, int map[map_size][map_size], int row, int col, SDL_Rect at_tile);

void load_snowy_ground_texture(SDL_Renderer* renderer, SDL_Texture* snowy_ground_tex, SDL_Rect destTile);
void load_ground_texture(SDL_Renderer* renderer, SDL_Rect destTile, int row, int column);
SDL_Texture* get_ground_texture(SDL_Renderer* renderer, const std::string& path);

void destroy_all_textures();