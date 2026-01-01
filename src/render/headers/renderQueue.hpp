#pragma once

#include <iostream>
#include <optional>

#include "textures.hpp"

struct RenderQueueItem {
    int renderOrder;
    SDL_FRect dstrect;
    SDL_FRect srcrect = { 0,0,0,0 };
    // if this is defined then texture method for rendering will be used
    std::optional<Texture*> texture;
    // function to call when rendering needs to be done, must contain 
    // everything it needs, except the renderer
    // try not to use this as thousands of lambda functions 
    // will cause a performance hit
    float alpha = 1.0f;

    std::optional<std::function<void(SDL_Renderer* renderer)>> customRender;
    RenderQueueItem(int renderOrder, SDL_FRect dstrect, Texture* texture, float alpha);
    RenderQueueItem(int renderOrder, SDL_FRect srcrect, SDL_FRect dstrect, Texture* texture, float alpha);
    RenderQueueItem(int renderOrder, std::function<void(SDL_Renderer* renderer)> customRender);
    void call_render(SDL_Renderer* renderer);
};

extern std::vector<RenderQueueItem> renderQueue;