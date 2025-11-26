#include "player.hpp"
#include "enemy.hpp"
#include "animation.hpp"
#include "textures.hpp"
#include <vector>

Uint32 AnimEnemy::lastUpdate;
int AnimEnemy::animRow = 1;
int AnimEnemy::animCol = 1;
int AnimEnemy::spriteEnum = 1;

void Enemy::animation(SDL_Renderer* renderer) {
    using namespace AnimEnemy;

    SDL_FRect srcRect;
    int animationSpeed = 32;
    const char* action = (movementVector.x==0 && movementVector.y==0) ? "idle" : "run";

    // select spriteEnum based on movement direction
    if (action == "run") {
        lastDirection = movementVector;
        if (movementVector.x == 1) { spriteEnum = Map::spider_run135_animation; }
        if (movementVector.x == -1) { spriteEnum = Map::spider_run315_animation; }
        if (movementVector.y == 1) { spriteEnum = Map::spider_run225_animation; }
        if (movementVector.y == -1) { spriteEnum = Map::spider_run45_animation; }
    } else {
        if (lastDirection.x == 1) { spriteEnum = Map::spider_idle135_animation; }
        if (lastDirection.x == -1) { spriteEnum = Map::spider_idle315_animation; }
        if (lastDirection.y == 1) { spriteEnum = Map::spider_idle225_animation; }
        if (lastDirection.y == -1) { spriteEnum = Map::spider_idle45_animation; }
    }
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
        lastUpdate = SDL_GetTicks();
        animCol++;
        // action
        if (action == "run") {
            if (animCol >= 4) {
                animRow++;
                animCol = 0;
            }
            if (animRow >= 4) {
                animRow = 0;
            }
        } else {
            if (animCol >= 6) {
                animRow++;
                animCol = 0;
            }
            if (animRow >= 4) {
                animRow = 0;
            }
        }
    }
    textureMap[spriteEnum].render(renderer, &srcRect, &dstRect);
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
