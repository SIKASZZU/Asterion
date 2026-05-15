#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <ctime>

#include "game_state.hpp"
#include "game.hpp"
#include "memory.hpp"
#include "offset.hpp"
#include "map.hpp"
#include "maze.hpp"
#include "raycast.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "isometric_calc.hpp"
#include "daylight.hpp"
#include "textures.hpp"
#include "vision.hpp"
#include "end.hpp"
#include "render.hpp"
#include "map.hpp"


namespace Game {

    bool init(GameState* gS) {

        std::cout << "C++ version: " << __cplusplus << '\n';

        const char* projectName = "Asterion";

        gS->screenWidth = 1600;
        gS->screenHeight = 960;

        std::cout << "Main thread id: " << std::this_thread::get_id() << '\n';

        std::jthread::id threadID = Raycast::start_worker();
        std::cout << "Raycast thread id: " << threadID << '\n';

        SDL_SetAppMetadata("Asterion", "1.0", "com.example.asterion");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            return false;
        }

        SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE
            | SDL_WINDOW_INPUT_FOCUS
            | SDL_WINDOW_MOUSE_FOCUS
            | SDL_WINDOW_HIGH_PIXEL_DENSITY;

        if (!SDL_CreateWindowAndRenderer(projectName, gS->screenWidth, gS->screenHeight, flags, &gS->s_window, &gS->s_renderer)) {
            SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
            return false;
        }

        SDL_GetWindowSize(gS->s_window, &gS->screenWidth, &gS->screenHeight);
        gS->s_keyState = SDL_GetKeyboardState(nullptr);

        load_textures(gS->s_renderer);
        return true;
    }

    void load_level(GameState* gS) {
        srand(static_cast<unsigned int>(std::time(nullptr)));
        PlayerNS::init(gS);

        MapNS::testMapEnvironment = false;
        MapGenerator::init();

        Vision::create_darkness(gS);

        Enemy e1(162, 162);
        Enemy e2(142, 162);
        e1.set_speed(320);   e1.set_size(MapNS::tileSize * 0.75f);
        e2.set_speed(320);   e2.set_size(MapNS::tileSize * 0.75f);
        enemyArray.push_back(e1);
        enemyArray.push_back(e2);
    }

    void handle_events(GameState* gS) {
        while (SDL_PollEvent(&gS->s_event)) {
            switch (gS->s_event.type) {
            case SDL_EVENT_QUIT:
                gS->isRunning = false;
                break;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                SDL_GetWindowSize(gS->s_window, &gS->screenWidth, &gS->screenHeight);
                break;
            case SDL_EVENT_KEY_DOWN:
                react_to_keyboard_down(gS, gS->s_event.key.key);
                break;
            case SDL_EVENT_KEY_UP:
                react_to_keyboard_up(gS, gS->s_event.key.key);
                break;
            }
        }

        PlayerNS::create_movementVector(gS->s_keyState);
    }

    void update(GameState* gS) {
        Uint64 now = SDL_GetTicks();
        Uint64 elapsedMS = now - gS->previousTick;
        gS->previousTick = now;
        gS->tickLag += elapsedMS;

        while (gS->tickLag >= gS->TICK_DELAY_MS) {
            gS->tickLag -= gS->TICK_DELAY_MS;
            gS->tickCount++;

            SDL_Point enemyTarget = {
                static_cast<int>((player.x + player.size / 2) / MapNS::tileSize),
                static_cast<int>((player.y + player.size / 2) / MapNS::tileSize)
            };
            for (auto& e : enemyArray)
                e.update(map, enemyTarget, gS->fixedDeltaTime);

            update_offset(gS, player);
            PlayerNS::update(gS->fixedDeltaTime, gS);
            Portal::has_entered();
            gS->s_terrain.update(gS->s_renderer);
            gS->s_terrain.calculate_miscellaneous(gS->fixedDeltaTime);
        }

        DaylightNS::update_daynight(elapsedMS);
    }

    void render(GameState* gS) {
        SDL_SetRenderDrawColor(gS->s_renderer, 0, 0, 0, 255);
        SDL_RenderClear(gS->s_renderer);

        gS->s_terrain.render(gS);
        Raycast::update();
        Vision::update(gS->s_renderer);

        if (Ending::start)
            Ending::update(gS->s_renderer);

        DaylightNS::draw(gS);

        // FPS counter
        Uint32 fpsNow = SDL_GetTicks();
        if (fpsNow - gS->fpsTimer > 400) {
            gS->fps = gS->frameCount * 1000.0f / (fpsNow - gS->fpsTimer);
            gS->frameCount = 0;
            gS->fpsTimer = fpsNow;
        }

        // Debug overlays
        DaylightNS::debug(gS->s_renderer);
        Game::debug(gS, gS->s_renderer);
        PlayerNS::debug(gS->s_renderer);

        SDL_RenderPresent(gS->s_renderer);
        gS->frameCount++;
    }

    bool is_running(GameState* gS) {
        return gS->isRunning;
    }

    void shutdown(GameState* gS) {
        Raycast::stop_worker();
        destroy_all_textures();
        SDL_DestroyRenderer(gS->s_renderer);
        SDL_DestroyWindow(gS->s_window);
        SDL_Quit();
    }

    void debug(GameState* gS, SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 100, 240, 240, 255);
        int x = 50, y = 60, lineHeight = 20;

        auto drawLine = [&](const std::string& text) {
            SDL_RenderDebugText(renderer, x, y, text.c_str());
            y += lineHeight;
            };

        drawLine("FPS: " + std::to_string((int)gS->fps));
        drawLine("MEMORY: " + std::to_string(printMemoryUsage()));
        drawLine("FLIPPED: " + std::string(offset.flipped ? "true" : "false"));
        drawLine("tile size: " + std::to_string(MapNS::tileSize));
    }

}

void rescale_world(float oldTs, float newTs) {
    int gx = static_cast<int>((player.x + player.size / 2.0f) / oldTs);
    int gy = static_cast<int>((player.y + player.size / 2.0f) / oldTs);
    player.x = gx * newTs;
    player.y = gy * newTs;

    float ratio = static_cast<float>(player.size / oldTs);
    player.size = newTs * ratio;
    player.grid = { gx, gy };
    SDL_FPoint iso = to_isometric_coordinate(player.x, player.y);
    player.rect.x = iso.x;
    player.rect.y = iso.y;

    for (auto& e : enemyArray)
        e.set_position({ static_cast<float>(e.grid.x) * newTs,
                         static_cast<float>(e.grid.y) * newTs });
}

void react_to_keyboard_down(GameState* gS, SDL_Keycode key) {

    switch (key) {
    case SDLK_SPACE:  player.jumping = true;  break;
    case SDLK_LSHIFT: player.shifting = true;  break;
    case SDLK_C: PlayerNS::toggle_collision(); break;
    case SDLK_R: Raycast::toggle(); break;

    case SDLK_T:
        offset.flipped = !offset.flipped;
        std::cout << "Camera flipped: " << offset.flipped << '\n';
        break;
    case SDLK_V:
        Vision::enabled = !Vision::enabled;
        std::cout << "Vision: " << Vision::enabled << '\n';
        break;
    case SDLK_K:
        debugText = !debugText;
        break;
    case SDLK_Q:
        gS->isRunning = false;
        break;

        // ── enemy ──────────────────────────────────────────────
    case SDLK_E:
        for (int i = 0; auto& e : enemyArray)
            e.spawn({ 155 + i++, 155 });
        break;
    case SDLK_O:
        player.state = PlayerState::Damage;
        for (auto& e : enemyArray)
            e.activity = EnemyActivity::Attack;
        break;

        // ── debug info ─────────────────────────────────────────
    case SDLK_F:
        std::cout << "offset: " << offset.x << " " << offset.y << '\n';
        for (const auto& e : enemyArray) {
            SDL_FPoint pos = e.get_position();
            SDL_Point  mv = e.get_movementVector();
            std::cout << "pos: " << pos.x << " " << pos.y
                << " grid: " << e.grid.x << " " << e.grid.y
                << " mv: " << mv.x << " " << mv.y
                << " activity: " << activityToString(e.activity)
                << " state: " << stateToString(e.state) << '\n';
        }
        break;

        // ── render radius ──────────────────────────────────────
    case SDLK_KP_PLUS: case SDLK_PLUS: MapNS::increase_radius(); break;
    case SDLK_KP_MINUS: case SDLK_MINUS: MapNS::decrease_radius(); break;

        // ── tile size ──────────────────────────────────────────
    case SDLK_PERIOD: MapNS::increase_tilesize(); break;
    case SDLK_COMMA: MapNS::decrease_tilesize(); break;

        // ── daylight ───────────────────────────────────────────
    case SDLK_Y: DaylightNS::toggle_daynight(); break;
    case SDLK_U: DaylightNS::decrease_time_speed(); break;
    case SDLK_I: DaylightNS::increase_time_speed(); break;

        // ── map ────────────────────────────────────────────────
    case SDLK_0: MapNS::change_map(); break;
    }
}

void react_to_keyboard_up(GameState* /*gS*/, SDL_Keycode key) {
    switch (key) {
    case SDLK_LSHIFT: player.shifting = false; break;
    default: break;
    }
}