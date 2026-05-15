#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include "terrain.hpp"

struct GameState {

    SDL_Window* s_window = nullptr;
    SDL_Renderer* s_renderer = nullptr;
    TerrainClass    s_terrain;
    SDL_Event       s_event;
    const bool* s_keyState = nullptr;

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

    const Uint64 TICK_DELAY_MS = 16;
    const float fixedDeltaTime = TICK_DELAY_MS / 1000.0f;

};