#include "player.hpp"
#include "enemy.hpp"
#include "animation.hpp"
#include "textures.hpp"

Uint32 AnimEnemy::lastUpdate;
int AnimEnemy::lastFrame = 0;
int AnimEnemy::animRow = 0;
int AnimEnemy::animCol = 0;

void Enemy::animation(SDL_Renderer* renderer) {
    using namespace AnimEnemy;
    const int sprite_width = 256;
    const int sprite_height = 256;
    SDL_FRect srcRect;
    int animationSpeed = 25;
    animCol = lastFrame % 16;

    if (movementVector.x == 1) { animRow = 1; }
    if (movementVector.x == -1) { animRow = 3; }

    if (movementVector.y == 1) { animRow = 2; }
    if (movementVector.y == -1) { animRow = 0; }

    srcRect.x = animCol * sprite_width;
    srcRect.y = animRow * sprite_height;
    srcRect.w = sprite_width;
    srcRect.h = sprite_height;

    SDL_FRect dstRect = rect;
    float scaleFactor = 1.0f;
    dstRect.w = sprite_width * scaleFactor;
    dstRect.h = sprite_height * scaleFactor;
    // center inside the original rect (e.g. tile)
    dstRect.x = rect.x + (rect.w - dstRect.w) * 0.5f;
    dstRect.y = rect.y + (rect.h - dstRect.h) * 0.5f;
    if (SDL_GetTicks() - lastUpdate > animationSpeed) {
        lastFrame++;
        lastUpdate = SDL_GetTicks();
    }
    texture_map[Map::SPIDER].render(renderer, &srcRect, &dstRect);
}


Uint32 AnimPlayer::lastUpdate = SDL_GetTicks();
int AnimPlayer::lastFrame = 0;
int AnimPlayer::lastRow = 0;

void animation_player(SDL_Renderer* renderer) {
    using namespace AnimPlayer;
    const int sprite_width = 64;
    const int sprite_height = 64;
    SDL_FRect srcRect;
    int col;

    // player standing
    if (player.movement_speed == 0) {

        col = lastFrame % 12;
        player.animation_speed = 100;
        if (player.last_movement_key == 'w' || player.last_movement_key == 'd') {
            lastRow = 2;
        }
        else if (player.last_movement_key == 's' || player.last_movement_key == 'a') {
            lastRow = 3;
        }
    }
    // player moving
    else {
        col = lastFrame % 8;
        float loweredMovementSpeed = player.movement_speed / PlayerNS::tilesPerSecond;
        player.shifting == true ? player.animation_speed = tile_size * 1.4 : player.animation_speed = tile_size * 0.8;
        if (player.movement_vector.x == 1) { lastRow = 0; }
        if (player.movement_vector.x == -1) { lastRow = 1; }
        if (player.movement_vector.y == 1) { lastRow = 1; }
        if (player.movement_vector.y == -1) { lastRow = 0; }
    }

    srcRect.x = col * sprite_width;
    srcRect.y = lastRow * sprite_height;
    srcRect.w = sprite_width;
    srcRect.h = sprite_height;
    if (SDL_GetTicks() - lastUpdate > player.animation_speed) {
        lastFrame++;
        lastUpdate = SDL_GetTicks();
    }
    texture_map[Map::PLAYER].render(renderer, &srcRect, &player.rect);
}
