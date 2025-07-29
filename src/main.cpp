#define SDL_MAIN_HANDLED

#include <ctime>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>


#include "game.hpp"
#include "player.hpp"
#include "textures.hpp"
#include "render.hpp"
#include "collision.hpp"
#include "raycast.hpp"
#include "raycast.hpp"
#include "vision.hpp"

int main(int argc, char* argv[]) {
    Offset offset = { 0, 0 };

    generate_map();
    srand(static_cast<unsigned int>(std::time(NULL)));
    static SDL_Window *window = NULL;
    static SDL_Renderer *renderer = NULL;
    // Initialize SDL video subsystem once, check error
    SDL_SetAppMetadata("Example Renderer Primitives", "1.0", "com.example.renderer-primitives");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/primitives", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GetWindowSize(window, &screen_width, &screen_height);

    load_textures(renderer);
    Vision::create_darkness(renderer);

    SDL_Event event;
    const bool* state = SDL_GetKeyboardState(nullptr);

    while (isRunning) {
        frame_start = SDL_GetTicks();
        SDL_GetMouseState(&mouse_x, &mouse_y);

        Uint32 elapsed_ticks = frame_start - previous_tick;
        previous_tick = frame_start;
        tick_lag += elapsed_ticks;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
                break;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode key = event.key.key;
                react_to_keyboard_down(key, player, offset, map);
            }
            else if (event.type == SDL_EVENT_KEY_UP) {
                SDL_Keycode key = event.key.key;
                react_to_keyboard_up(key, player);
            }
        }

        react_to_keyboard_state(state, player);

        frame_count++;

        while (tick_lag > tick_delay) {
            tick_count++;
            tick_lag -= tick_delay;

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderClear(renderer);

            update_offset(offset, player);
            load_render(renderer, offset, player);

            update_player(map, offset, renderer);
            Raycast::update(renderer, offset, map);

            Vision::draw_overlay(renderer, offset);

            SDL_RenderPresent(renderer);

            if (SDL_GetTicks() - fps_timer >= 1000) {
                fps = frame_count * 1000.0f / (frame_start - fps_timer);
                ticks_per_second = tick_count * 1000.0f / (frame_start - tick_timer);

                fps_timer = SDL_GetTicks();
                tick_timer = SDL_GetTicks();
                frame_count = 0;
                tick_count = 0;
            }
        }

        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < 16) { SDL_Delay(16 - frame_time); }
    }

    destroy_all_textures();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}