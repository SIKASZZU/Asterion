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
    const int spriteWidth = 256;
    const int spriteHeight = 256;
    SDL_FRect srcRect;
    int animationSpeed = 25;
    animCol = lastFrame % 16;

    if (movementVector.x == 1) { animRow = 1; }
    if (movementVector.x == -1) { animRow = 3; }

    if (movementVector.y == 1) { animRow = 2; }
    if (movementVector.y == -1) { animRow = 0; }

    srcRect.x = animCol * spriteWidth;
    srcRect.y = animRow * spriteHeight;
    srcRect.w = spriteWidth;
    srcRect.h = spriteHeight;

    SDL_FRect dstRect = rect;
    float scaleFactor = 1.0f;
    dstRect.w = spriteWidth * scaleFactor;
    dstRect.h = spriteHeight * scaleFactor;
    // center inside the original rect (e.g. tile)
    dstRect.x = rect.x + (rect.w - dstRect.w) * 0.5f;
    dstRect.y = rect.y + (rect.h - dstRect.h) * 0.5f;
    if (SDL_GetTicks() - lastUpdate > animationSpeed) {
        lastFrame++;
        lastUpdate = SDL_GetTicks();
    }
    textureMap[Map::SPIDER].render(renderer, &srcRect, &dstRect);
}


Uint32 AnimPlayer::lastUpdate = SDL_GetTicks();
int AnimPlayer::lastFrame = 0;
int AnimPlayer::lastRow = 0;

void animation_player(SDL_Renderer* renderer) {
    using namespace AnimPlayer;
    const int spriteWidth = 64;
    const int spriteHeight = 64;
    SDL_FRect srcRect;
    int col;

    // player standing
    if (player.movementSpeed == 0) {

        col = lastFrame % 12;
        player.animationSpeed = 100;
        if (player.lastMovementKey == 'w' || player.lastMovementKey == 'd') {
            lastRow = 2;
        }
        else if (player.lastMovementKey == 's' || player.lastMovementKey == 'a') {
            lastRow = 3;
        }
    }
    // player moving
    else {
        col = lastFrame % 8;
        float loweredMovementSpeed = player.movementSpeed / PlayerNS::tilesPerSecond;
        player.shifting == true ? player.animationSpeed = tileSize * 1.4 : player.animationSpeed = tileSize * 0.8;
        if (player.movementVector.x == 1) { lastRow = 0; }
        if (player.movementVector.x == -1) { lastRow = 1; }
        if (player.movementVector.y == 1) { lastRow = 1; }
        if (player.movementVector.y == -1) { lastRow = 0; }
    }

    srcRect.x = col * spriteWidth;
    srcRect.y = lastRow * spriteHeight;
    srcRect.w = spriteWidth;
    srcRect.h = spriteHeight;
    if (SDL_GetTicks() - lastUpdate > player.animationSpeed) {
        lastFrame++;
        lastUpdate = SDL_GetTicks();
    }
    textureMap[Map::PLAYER].render(renderer, &srcRect, &player.rect);
}
