#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <ctime>

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

/* ── globals ──────────────────────────────────────────────── */

bool        isRunning = true;
float       mouse_x = 0;
float       mouse_y = 0;
int         screenWidth = 0;
int         screenHeight = 0;

Uint32      frameCount = 0;
float       fps = 0.0f;
Uint32      fpsTimer = 0;
Uint32      frameTime = 0;

Uint32      tickLag = 0;
const int   tickrate = 60;
int         tickCount = 0;
float       tps = 0.0f;
Uint64      previousTick = SDL_GetTicks();

int         renderRadius = 20;
float       tileSize = 100.0f;
bool        testMapEnvironment = false;

const Uint64 TICK_DELAY_MS = 16;
const float fixedDeltaTime = TICK_DELAY_MS / 1000.0f;

int pathEndX = -1, pathEndY = -1;
int pathStartX = -1, pathStartY = -1;
bool v_pressed = false;

/* ── file-scope state (private to game.cpp) ───────────────── */

static SDL_Window* s_window = nullptr;
static SDL_Renderer* s_renderer = nullptr;
static TerrainClass  s_terrain;
static SDL_Event     s_event;
static const bool* s_keyState = nullptr;

namespace Game {

    bool init() {
        const char* projectName = "Asterion";
        screenWidth = 1600; screenHeight = 960;

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

        if (!SDL_CreateWindowAndRenderer(projectName, screenWidth, screenHeight, flags, &s_window, &s_renderer)) {
            SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
            return false;
        }

        SDL_GetWindowSize(s_window, &screenWidth, &screenHeight);
        s_keyState = SDL_GetKeyboardState(nullptr);

        load_textures(s_renderer);
        return true;
    }

    void load_level() {
        srand(static_cast<unsigned int>(std::time(nullptr)));
        PlayerNS::init();

        testMapEnvironment = false;
        generate_map();

        if (testMapEnvironment)
            map[165][165] = Map::CAMPFIRE;

        Vision::create_darkness(s_renderer);

        Enemy e1(162, 162);
        Enemy e2(142, 162);
        e1.set_speed(320);   e1.set_size(tileSize * 0.75f);
        e2.set_speed(320);   e2.set_size(tileSize * 0.75f);
        enemyArray.push_back(e1);
        enemyArray.push_back(e2);
    }

    void handle_events() {
        while (SDL_PollEvent(&s_event)) {
            switch (s_event.type) {
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                SDL_GetWindowSize(s_window, &screenWidth, &screenHeight);
                break;
            case SDL_EVENT_KEY_DOWN:
                react_to_keyboard_down(s_event.key.key);
                break;
            case SDL_EVENT_KEY_UP:
                react_to_keyboard_up(s_event.key.key);
                break;
            }
        }

        PlayerNS::create_movementVector(s_keyState);
    }

    void update() {
        Uint64 now = SDL_GetTicks();
        Uint64 elapsedMS = now - previousTick;
        previousTick = now;
        tickLag += elapsedMS;

        while (tickLag >= TICK_DELAY_MS) {
            tickLag -= TICK_DELAY_MS;
            tickCount++;

            SDL_Point enemyTarget = {
                static_cast<int>((player.x + player.size / 2) / tileSize),
                static_cast<int>((player.y + player.size / 2) / tileSize)
            };
            for (auto& e : enemyArray)
                e.update(map, enemyTarget, fixedDeltaTime);

            update_offset(player);
            PlayerNS::update(fixedDeltaTime);
            Portal::has_entered();
            s_terrain.update(s_renderer);
            s_terrain.calculate_miscellaneous(fixedDeltaTime);
        }

        DaylightNS::update_daynight(elapsedMS);
    }

    void render() {
        SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
        SDL_RenderClear(s_renderer);

        s_terrain.render(s_renderer);
        Raycast::update(s_renderer);
        Vision::update(s_renderer);

        if (Ending::start)
            Ending::update(s_renderer);

        DaylightNS::draw(s_renderer);

        // FPS counter
        Uint32 fpsNow = SDL_GetTicks();
        if (fpsNow - fpsTimer > 400) {
            fps = frameCount * 1000.0f / (fpsNow - fpsTimer);
            frameCount = 0;
            fpsTimer = fpsNow;
        }

        // Debug overlays
        DaylightNS::debug(s_renderer);
        Game::debug(s_renderer);
        PlayerNS::debug(s_renderer);

        SDL_RenderPresent(s_renderer);
        frameCount++;
    }

    bool is_running() {
        return isRunning;
    }

    void shutdown() {
        Raycast::stop_worker();
        destroy_all_textures();
        SDL_DestroyRenderer(s_renderer);
        SDL_DestroyWindow(s_window);
        SDL_Quit();
    }

    void debug(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 100, 240, 240, 255);
        int x = 50, y = 60, lineHeight = 20;

        auto drawLine = [&](const std::string& text) {
            SDL_RenderDebugText(renderer, x, y, text.c_str());
            y += lineHeight;
            };

        drawLine("FPS: " + std::to_string((int)fps));
        drawLine("MEMORY: " + std::to_string(printMemoryUsage()));
        drawLine("FLIPPED: " + std::string(offset.flipped ? "true" : "false"));
    }

}

void rescale_world(float oldTs, float newTs) {
    int gx = static_cast<int>((player.x + player.size / 2.0f) / oldTs);
    int gy = static_cast<int>((player.y + player.size / 2.0f) / oldTs);
    player.x = gx * newTs;
    player.y = gy * newTs;
    player.size = newTs;
    player.grid = { gx, gy };
    SDL_FPoint iso = to_isometric_coordinate(player.x, player.y);
    player.rect.x = iso.x;
    player.rect.y = iso.y;

    for (auto& e : enemyArray)
        e.set_position({ static_cast<float>(e.grid.x) * newTs,
                         static_cast<float>(e.grid.y) * newTs });
}
void react_to_keyboard_down(SDL_Keycode key) {

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
        v_pressed = !v_pressed;
        std::cout << "Vision: " << v_pressed << '\n';
        break;
    case SDLK_K:
        debugText = !debugText;
        break;
    case SDLK_Q:
        isRunning = false;
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
void react_to_keyboard_up(SDL_Keycode key) {
    switch (key) {
    case SDLK_LSHIFT: player.shifting = false; break;
    default: break;
    }
}