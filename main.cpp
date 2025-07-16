#define SDL_MAIN_HANDLED

#include <ctime>
#include <SDL2/SDL_image.h>

#include "game.h"
#include "player.h"
#include "textures.h"
#include "render.h"
#include "collision.h"

int main(int argc, char* argv[]) {
    SDL_SetMainReady();  // compiler ja windows bitching. Yritab muidu SDL maini kasutada
    Offset offset = { 0, 0 };

    generate_map();
    // print_map(map);

    srand(static_cast<unsigned int>(std::time(NULL)));  // random numbrite jaoks

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Asterion",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 720, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    load_textures(renderer);

    SDL_Event event;
    const Uint8* state = SDL_GetKeyboardState(NULL);

    /* game loop */
    while (isRunning) {
        frame_start = SDL_GetTicks();  // framerate
        SDL_GetMouseState(&mouse_x, &mouse_y);  // a 32-bit button bitmask of the current button state. Mis tra asi see on?

        Uint32 elapsed_ticks = frame_start - previous_tick;
        previous_tick = frame_start;
        tick_lag += elapsed_ticks;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
                break;
            }
            else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                react_to_keyboard_down(key, player, offset, map);
            }
            else if (event.type == SDL_KEYUP) {
                SDL_Keycode key = event.key.keysym.sym;
                react_to_keyboard_up(key, player);
            }
            react_to_keyboard_state(state, player);
        }

        frame_count++;

        while (tick_lag > tick_delay) {
            /* ticki sees peavad olema k6ik, mis fpsiga muutuks kiiremaks ja annaks eelise vs teise m2ngija suhtes. */
            tick_count++;

            tick_lag -= tick_delay;  // update tickrate

            /* Render begin*/
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Clear with blue!
            SDL_RenderClear(renderer);  // enne uut framei, t6mba plats puhtaks

            load_render(renderer, offset, player);

            update_offset(offset, player, window);
            update_player(map, offset, state, renderer);

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // enne millegi renderimist, peab lisama rendererile colori.        
            SDL_RenderPresent(renderer);  // dispay new frame

            /* Framerate and tickrate updating */
            if (SDL_GetTicks() - fps_timer >= 1000) {
                fps = frame_count * 1000.0f / (frame_start - fps_timer);
                ticks_per_second = tick_count * 1000.0f / (frame_start - tick_timer);

                // std::cout << fps << " fps, " << ticks_per_second << " tps" << std::endl;

                fps_timer = SDL_GetTicks();
                tick_timer = SDL_GetTicks();
                frame_count = 0;
                tick_count = 0;
            }
        }

        /* Framerate cap */
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < 16) { SDL_Delay(16 - frame_time); }
    }

    // Cleanup if isRunning == false
    destroy_all_textures();
    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
