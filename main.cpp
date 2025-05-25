#include <iostream>
#include <SDL_image.h>
#include "game.h"

int main(int argc, char* argv[]) {

    Offset offset = {0, 0};
    
    generate_random_map(map, 0, 2);
    // print_map(map);

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("SDL cpp",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  
    /* textures */
    SDL_Texture* ground_tex = IMG_LoadTexture(renderer, "resources/snowy_ground.png");
    SDL_Texture* tree_tex   = IMG_LoadTexture(renderer, "resources/snowy_tree.png");

    SDL_GetWindowSize(window, &win_width, &win_height);

    player.x = (win_width / 2) - (player.size / 2);
    player.y = (win_height / 2) - (player.size / 2);
    SDL_Rect rect{player.x, player.y, player.size, player.size};
    
    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    /* game loop */
    while (isRunning) {
        frame_start = SDL_GetTicks();  // framerate
        

        Uint32 current_tick = SDL_GetTicks(); // tickrate
        Uint32 elapsed_ticks = current_tick - previous_tick;
        previous_tick = current_tick;
        tick_lag += elapsed_ticks;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false; 
                break;
            }
            if (state[SDL_SCANCODE_F]) {
                int player_tile_x = player.x / tile_size;
                int player_tile_y = player.y / tile_size;
                int tile_value = map[player_tile_y][player_tile_x];
                std::cout << player_tile_y << ' ' << player_tile_x << ' ' << tile_value << '\n';
            }
        }
        
        frame_count++;
        
        while (tick_lag > tick_delay) {
            tick_count++;

            tick_lag -= tick_delay;  // update tickrate
            
            update_player(offset, state);
            update_offset(offset, win_width, win_height);
            
            /* Render begin*/
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Clear with black
            SDL_RenderClear(renderer);  // clear previous frame
            
            render_map(renderer, tile_size, offset, ground_tex, tree_tex);
            
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // enne millegi renderimist, peab lisama rendererile colori.        
            SDL_RenderFillRect(renderer, &rect);  // player
            SDL_RenderPresent(renderer);  // dispay new frame

            // /* Framerate and tickrate updating */
            if (SDL_GetTicks() - fps_timer >= 1000) {
                fps = frame_count * 1000.0f / (frame_start - fps_timer);
                ticks_per_second = tick_count * 1000.0f / (frame_start - tick_timer);

                std::cout << fps << " fps, " << ticks_per_second << " tps" << std::endl;

                fps_timer = SDL_GetTicks();
                tick_timer = SDL_GetTicks();
                frame_count = 0;
                tick_count = 0;
            }
        }

        /* Framerate cap */
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < 8) { SDL_Delay(8 - frame_time); }

        std::cout << std::endl;

    }

    // Cleanup if isRunning == false
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(ground_tex);
    SDL_DestroyTexture(tree_tex);
    SDL_Quit();
    IMG_Quit();
    return 0;
}
