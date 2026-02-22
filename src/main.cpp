#define SDL_MAIN_HANDLED

#include <ctime>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <algorithm>
#include <array>
#include <thread>

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
#include "memory.hpp"
#include "daylight.hpp"
#include <thread>
#include <iostream>

int main(int argc, char* argv[]) {

    std::cout << "\n201103L => C++11\n201402L => C++14\n201703L => C++17\n202002L => C++20\n202302L => C++23\nCPP VERSION: " << __cplusplus << std::endl;

    std::cout << "Main thread id: " << std::this_thread::get_id() << '\n';
    // Start raycast worker thread (background computations)
    std::jthread::id threadID = Raycast::start_worker();
    std::cout << "Raycast thread id: " << threadID << '\n';

    static SDL_Window* window = NULL;
    static SDL_Renderer* renderer = NULL;
    // Initialize SDL video subsystem once, check error
    SDL_SetAppMetadata("Example Renderer Primitives", "1.0", "com.example.renderer-primitives");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    // 1280	720, https://en.wikipedia.org/wiki/16:9_aspect_ratio
    if (!SDL_CreateWindowAndRenderer("Asterion", 1024, 768, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GetWindowSize(window, &screenWidth, &screenHeight);

    load_textures(renderer);

    srand(static_cast<unsigned int>(std::time(NULL)));
    generate_map();
    map[165][160] = Map::WALL_CUBE;

    TerrainClass terrain;
    Vision::create_darkness(renderer);

    SDL_Event event;
    const bool* state = SDL_GetKeyboardState(nullptr);

    Enemy enemy(162, 162);
    map[162][162] = Map::ERROR_CUBE;
    enemy.set_speed(320);
    enemy.set_size(tileSize * 0.75f);

    enemyArray.push_back(enemy);

    while (isRunning) {
        Uint64 now = SDL_GetTicks();
        Uint64 elapsedMS = now - previousTick;
        previousTick = now;
        tickLag += elapsedMS;

        // Input (always real-time)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) isRunning = false;
            else if (event.type == SDL_EVENT_KEY_DOWN)
                react_to_keyboard_down(event.key.key, player, offset, map);
            else if (event.type == SDL_EVENT_KEY_UP)
                react_to_keyboard_up(event.key.key, player);
        }
        react_to_keyboard_state(state);

        // tickrate specific
        while (tickLag >= TICK_DELAY_MS) {
            tickLag -= TICK_DELAY_MS;
            tickCount++;

            // (use fixedDeltaTime!)
            SDL_Point enemyTargetGrid = {
                static_cast<int>((player.x + player.size / 2) / tileSize),
                static_cast<int>((player.y + player.size / 2) / tileSize)
            };
            for (auto& e : enemyArray) {
                e.update(map, enemyTargetGrid, fixedDeltaTime);
            }
            PlayerNS::update(map, offset, renderer, fixedDeltaTime);
            update_offset(player);

            Portal::has_entered();
            terrain.calculate_miscellaneous(fixedDeltaTime);
        }
        DaylightNS::update_daynight(elapsedMS);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        terrain.render(renderer);
        Raycast::update(renderer, offset);
        Vision::update(renderer, offset);
        if (Ending::start) Ending::update(renderer);
        DaylightNS::draw(renderer);
        // FPS counter
        Uint32 fpsNow = SDL_GetTicks();
        if (fpsNow - fpsTimer > 400) {
            fps = frameCount * 1000.0f / (fpsNow - fpsTimer);
            frameCount = 0;
            fpsTimer = fpsNow;
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer, 50, 110, ("FPS: " + std::to_string((int)fps)).c_str());
        SDL_RenderDebugText(renderer, 50, 130, (std::string("Mem: ") + std::to_string(printMemoryUsage())).c_str());
        DaylightNS::debug(renderer);
        SDL_RenderPresent(renderer);
        frameCount++;
    }

    // stop background raycast worker
    Raycast::stop_worker();
    destroy_all_textures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}