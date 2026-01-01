#include "renderQueue.hpp"

RenderQueueItem::RenderQueueItem(int renderOrder, SDL_FRect dstrect, Texture* texture, float alpha = 1.0f) {
    this->renderOrder = renderOrder;
    this->dstrect = dstrect;
    this->texture = texture;
    this->alpha = alpha;
}
RenderQueueItem::RenderQueueItem(int renderOrder, SDL_FRect srcrect, SDL_FRect dstrect, Texture* texture, float alpha = 1.0f) {
    this->renderOrder = renderOrder;
    this->dstrect = dstrect;
    this->srcrect = srcrect;
    this->texture = texture;
    this->alpha = alpha;
}
RenderQueueItem::RenderQueueItem(int renderOrder, std::function<void(SDL_Renderer* renderer)> customRender) {
    this->renderOrder = renderOrder;
    this->customRender = customRender;
}

void RenderQueueItem::call_render(SDL_Renderer* renderer) {
    if (this->texture.has_value()) {
        SDL_SetTextureAlphaMod((*texture)->get_texture(), static_cast<Uint8>(alpha * 255));
        if (srcrect.h != 0 && srcrect.w != 0) {
            return this->texture.value()->render(renderer, &this->srcrect, &this->dstrect);
        }
        else {
            return this->texture.value()->render(renderer, &this->dstrect);
        }
    }
    if (this->customRender.has_value()) {
        return this->customRender.value()(renderer);
    }
    throw std::logic_error("texture or render function needed for rendering");
}
std::vector<RenderQueueItem> renderQueue;