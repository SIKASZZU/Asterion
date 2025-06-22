#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <unordered_map>

// Define the global variables here, et mainis probleeme ei tekiks.
SDL_Texture* ground_tex = nullptr;
SDL_Texture* tree_tex = nullptr;
SDL_Texture* numbers_tex = nullptr;
SDL_Texture* wall_tex = nullptr;


const int texture_width = 16;
const int texture_height = 16;
const int numbers_tex_y = 0;
const int wall_tex_y = 0;

void load_textures(SDL_Renderer* renderer) {

    /* textures */
    ground_tex  = IMG_LoadTexture(renderer, "resources/snowy_ground.png");
    tree_tex    = IMG_LoadTexture(renderer, "resources/snowy_tree.png");
    numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
    wall_tex    = IMG_LoadTexture(renderer, "resources/wall.png");

    /* Puhasta tekstuuride 22ri et ei oleks blurry */
    SDL_SetTextureScaleMode(ground_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(tree_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest);
    SDL_SetTextureScaleMode(wall_tex, SDL_ScaleModeNearest);

}


void destroy_all_textures() {
    SDL_DestroyTexture(ground_tex);
    SDL_DestroyTexture(tree_tex);
    SDL_DestroyTexture(numbers_tex);
    SDL_DestroyTexture(wall_tex);
}


void load_specific_number(SDL_Renderer* renderer, int number, SDL_Rect at_tile) {
    if (number < 0 || number > 9) {
        std::cerr << "Invalid number: " << number << " Numbers combined not added yet" << std::endl;
        return;
    }

    // Load the numbers sprite sheet (0 - 9, each 16px wide)
    if (!numbers_tex) {
        SDL_Texture* numbers_tex = IMG_LoadTexture(renderer, "resources/numbers.png");
        SDL_SetTextureScaleMode(numbers_tex, SDL_ScaleModeNearest); // CLEAN the texture
    }

    SDL_Rect number_rect = {number * texture_width, numbers_tex_y, texture_width, texture_height};
    SDL_RenderCopy(renderer, numbers_tex, &number_rect, &at_tile);
}


void draw_wall(SDL_Renderer* renderer, SDL_Texture* wall_tex, const bool neighbors[3][3], SDL_Rect destTile) {
    /* Summa (var int mask) j2rgi t6mbab maskToXY arraist walli info, et otsida seda tekstuurilt */
 
    int mask = 0;
    if (neighbors[0][1]) mask |= 1;  // North
    if (neighbors[1][2]) mask |= 2;  // East
    if (neighbors[2][1]) mask |= 4;  // South
    if (neighbors[1][0]) mask |= 8;  // West

    // Define how each mask maps to (column, row) in your texture.
    // Adjust the pairs if your actual texture layout differs.
    static const std::pair<int,int> maskToXY[16] = {
                  /* !!!!!KUST POOL CONNECTION TEKIB!!!!!! vahel on vale idk */
        {0, 0},   // 0: single block
        {4, 2},   // 1: north XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {5, 0},   // 2: west XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {1, 2},   // 3: west+north XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {4, 0},   // 4: south XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {4, 1},   // 5: north+south
        {1, 0},   // 6: west+south
        {4, 4},   // 7: north+west+south
        {7, 0},   // 8: east XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {3, 2},   // 9: east+north XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {6, 0},   // 10: east+west (span)
        {1, 4},   // 11: north+east+west
        {3, 0},   // 12: east+south XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 
        {10, 4},   // 13: north+south+east XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        {7, 4},   // 14: east+south+west Y
        {9, 1}    // 15: all four sides connected (intersection)
        
    };


    // single block
    // 0-16, 0-16

    // 1 tulp vasak aar
    // 0-16, 16-32,
    // 16-32, 16-32
    // 32-48, 16-32,

    // 2 tulp keskmine osa
    // 0-16, 32-48
    // 16-32, 32-48
    // 32-48, 32-48
   
    // 3 tulp parem aar
    // 0-16,  48-62 
    // 16-32, 48-62
    // 32-48, 48-62

    // yksik vertikaalne
    // 0-16,  62-78 
    // 16-32, 62-78
    // 32-48, 62-78

    // yksik horisontaalne
    // 78-94, 0-16
    // 94-110, 0-16
    // 110-126, 0-16

    auto [col, row] = maskToXY[mask];
    SDL_Rect src = {
        col * 16,      // x-offset in texture
        row * 16,      // y-offset in texture
        16, 16         // width & height of each tile
    };
    SDL_RenderCopy(renderer, wall_tex, &src, &destTile);
}


void load_wall_texture(SDL_Renderer* renderer, SDL_Texture* wall_tex,
                       int map[100][100], int row, int col,
                       SDL_Rect destTile) {
    bool neighbors[3][3] = {{false}};
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            int r = row + dr, c = col + dc;
            if (r >= 0 && r < 100 && c >= 0 && c < 100 && map[r][c] == 9) {
                neighbors[dr + 1][dc + 1] = true;
            }
        }
    }
    neighbors[1][1] = true; // always set center true

    /* print out wall neighbours */
    // for (int dr = 0; dr < 3; ++dr) {
    //     for (int dc = 0; dc < 3; ++dc) {
    //         std::cout << (neighbors[dr][dc] ? '1' : '0') << ' ';
    //     }
    //     std::cout << '\n';
    // }
    // std::cout << "---\n";

    draw_wall(renderer, wall_tex, neighbors, destTile);
}