#define SDL_MAIN_HANDLED

#include <ctime>
#include <cmath>
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
#include "memory.hpp"

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

    Enemy enemy(162, 162);
    map[162][162] = Map::ERROR_CUBE;
    enemy.set_speed(250);
    enemy.set_size(tileSize * 0.75f);

    enemyArray.push_back(enemy);

    while (isRunning) {
        Uint64 now = SDL_GetTicks();
        Uint64 elapsedMS = now - previousTick;
        previousTick = now;
        tickLag += elapsedMS;

        // advance day/night clock (visual effect)
        update_daynight(elapsedMS);

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
                static_cast<int>(player.x / tileSize),
                static_cast<int>(player.y / tileSize)
            };

            PlayerNS::update(map, offset, renderer, fixedDeltaTime);

            for (auto& e : enemyArray) {
                e.update(map, enemyTargetGrid, fixedDeltaTime);
            }

            update_offset(player);
            Portal::has_entered();

        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        terrain.render(renderer);

        Raycast::update(renderer, offset, map);
        Vision::update(renderer, offset);

        if (Ending::start) Ending::update(renderer);

        // Draw day/night overlay: tint the final framebuffer according to timeOfDay
        if (dayNightEnabled) {
            // brightness oscillates with cosine: 1.0 = full day, 0.0 = full night
            // get centralized brightness (1=day,0=night), respects inversion
            float brightness = get_day_brightness();
            // compute alpha for night tint (0..maxAlpha)
            const float maxNightAlpha = 200.0f; // how dark the night can get
            float nightAlpha = (1.0f - brightness) * maxNightAlpha;
            if (nightAlpha > 0.5f) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                // bluish night tint
                Uint8 a = static_cast<Uint8>(std::clamp(nightAlpha, 0.0f, 255.0f));
                SDL_SetRenderDrawColor(renderer, 12, 24, 64, a);
                SDL_FRect full = { 0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight) };
                SDL_RenderFillRect(renderer, &full);
            }
        }

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
        // Day/night debug status
        {
            int hours = static_cast<int>(timeOfDay * 24.0f) % 24;
            int minutes = static_cast<int>((timeOfDay * 24.0f - hours) * 60.0f);
            char buf[96];
            snprintf(buf, sizeof(buf), "DayNight: %s  Time: %02d:%02d  Cycle: %.0fs",
                dayNightEnabled ? "On" : "Off",
                hours, minutes, dayLengthSeconds);
            SDL_RenderDebugText(renderer, 50, 150, buf);
            float brightness = dayNightEnabled ? 0.5f * (1.0f + cos(2.0 * PI * timeOfDay)) : 1.0f;
            char buf2[64];
            snprintf(buf2, sizeof(buf2), "Brightness: %.2f", brightness);
            SDL_RenderDebugText(renderer, 50, 170, buf2);
        }
        SDL_RenderPresent(renderer);
        frameCount++;
    }

    destroy_all_textures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}