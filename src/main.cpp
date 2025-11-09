#define SDL_MAIN_HANDLED

#include <ctime>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <array>

#include "offset.hpp"
#include "player.hpp"
#include "game.hpp"
#include "textures.hpp"
#include "render.hpp"
#include "collision.hpp"
#include "raycast.hpp"
#include "vision.hpp"
#include "enemy.hpp"
#include "end.hpp"

int main(int argc, char* argv[]) {

    generate_map();
    map[165][160] = Map::WALL_CUBE;
    srand(static_cast<unsigned int>(std::time(NULL)));
    static SDL_Window* window = NULL;
    static SDL_Renderer* renderer = NULL;
    // Initialize SDL video subsystem once, check error
    SDL_SetAppMetadata("Example Renderer Primitives", "1.0", "com.example.renderer-primitives");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    // 1280	720, https://en.wikipedia.org/wiki/16:9_aspect_ratio
    if (!SDL_CreateWindowAndRenderer("Asterion", 1600, 900, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GetWindowSize(window, &screenWidth, &screenHeight);
    load_textures(renderer);
    TerrainClass terrain;
    Vision::create_darkness(renderer);
    SDL_Event event;
    const bool* state = SDL_GetKeyboardState(nullptr);

    Enemy enemy(162, 162, tileSize);
    enemyArray.push_back(enemy);

    while (isRunning) {
        Uint64 frameStart = SDL_GetTicks();
        SDL_GetMouseState(&mouse_x, &mouse_y);
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
        react_to_keyboard_state(state);

        // frameTime = SDL_GetTicks() - frameStart;
        // if (frameTime < 16) { SDL_Delay(16 - frameTime); }

        Uint64 elapsedTicks = frameStart - previousTick;
        previousTick = frameStart;
        tickLag += elapsedTicks;
        frameCount++;

        float deltaTime = elapsedTicks / 1000.0f;
        PlayerNS::update(map, offset, renderer, deltaTime);
        update_offset(player);

        while (tickLag > tickDelay) {
            tickCount++;
            tickLag -= tickDelay;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Point enemyTarget = {
                static_cast<int>(player.x),
                static_cast<int>(player.y)
            };
            for (auto& e : enemyArray) {
                e.update(map, enemyTarget);
            }

            if (Ending::start) {
                Ending::update(renderer);
                SDL_RenderPresent(renderer);
                break;
            }

            terrain.render(renderer);

            for (auto& e : enemyArray) {
                e.render(renderer, offset);
            }

            Raycast::update(renderer, offset, map);
            Vision::update(renderer, offset);
            Portal::has_entered();

            SDL_RenderPresent(renderer);

            Uint32 now = SDL_GetTicks();
            Uint32 elapsed = now - fpsTimer;
            if (elapsed > 1000) {
                fps = frameCount * 1000.0f / elapsed;
                tps = tickCount * 1000.0f / elapsed;
                std::cout << std::endl;
                std::cout << "elapsed:    " << elapsed << '\n';
                std::cout << "fps:        " << fps << "\n";
                std::cout << "frameCount: " << frameCount << "\n";
                std::cout << "tps:        " << tps << "\n";
                std::cout << "deltaTime: " << deltaTime << '\n';
                fpsTimer = now;
                frameCount = 0;
                tickCount = 0;
            }
        }
    }

    destroy_all_textures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}