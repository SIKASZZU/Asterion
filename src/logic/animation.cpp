#include "player.hpp"
#include "enemy.hpp"
#include "animation.hpp"
#include "textures.hpp"
#include <cstring>
#include <vector>

Uint32 AnimEnemy::lastUpdate;
int AnimEnemy::animRow = 1;
int AnimEnemy::animCol = 1;
int AnimEnemy::spriteEnum = 1;

void Enemy::animation(SDL_Renderer* renderer, const char* activity) {
    using namespace AnimEnemy;

    SDL_FRect srcRect;
    int animationSpeed = 48;

    if (strcmp(activity, "chasing") == 0) {
        lastDirection = movementVector;
        if (movementVector.x == 1 && movementVector.y == 0) { spriteEnum = Map::spider_run135_animation; }
        if (movementVector.x == -1 && movementVector.y == 0) { spriteEnum = Map::spider_run315_animation; }
        if (movementVector.y == 1 && movementVector.x == 0) { spriteEnum = Map::spider_run225_animation; }
        if (movementVector.y == -1 && movementVector.x == 0) { spriteEnum = Map::spider_run45_animation; }

        if (movementVector.y == -1 && movementVector.x == 1) { spriteEnum = Map::spider_run90_animation; }
        if (movementVector.x == 1 && movementVector.y == 1) { spriteEnum = Map::spider_run180_animation; }
        if (movementVector.y == 1 && movementVector.x == -1) { spriteEnum = Map::spider_run270_animation; }
        if (movementVector.x == -1 && movementVector.y == -1) { spriteEnum = Map::spider_run0_animation; }
    }
    else if (strcmp(activity, "roaming") == 0) {
        lastDirection = movementVector;
        animationSpeed = 48;
        if (movementVector.x == 1 && movementVector.y == 0) { spriteEnum = Map::spider_walk135_animation; }
        if (movementVector.x == -1 && movementVector.y == 0) { spriteEnum = Map::spider_walk315_animation; }
        if (movementVector.y == 1 && movementVector.x == 0) { spriteEnum = Map::spider_walk225_animation; }
        if (movementVector.y == -1 && movementVector.x == 0) { spriteEnum = Map::spider_walk45_animation; }

        if (movementVector.y == -1 && movementVector.x == 1) { spriteEnum = Map::spider_walk90_animation; }
        if (movementVector.x == 1 && movementVector.y == 1) { spriteEnum = Map::spider_walk180_animation; }
        if (movementVector.y == 1 && movementVector.x == -1) { spriteEnum = Map::spider_walk270_animation; }
        if (movementVector.x == -1 && movementVector.y == -1) { spriteEnum = Map::spider_walk0_animation; }
    }
    else {
        animationSpeed = 72;
        if (lastDirection.x == 1 && movementVector.y == 0) { spriteEnum = Map::spider_idle135_animation; }
        if (lastDirection.x == -1 && movementVector.y == 0) { spriteEnum = Map::spider_idle315_animation; }
        if (lastDirection.y == 1 && movementVector.x == 0) { spriteEnum = Map::spider_idle225_animation; }
        if (lastDirection.y == -1 && movementVector.x == 0) { spriteEnum = Map::spider_idle45_animation; }

        if (lastDirection.y == -1 && movementVector.x == 1) { spriteEnum = Map::spider_idle90_animation; }
        if (lastDirection.x == 1 && movementVector.y == 1) { spriteEnum = Map::spider_idle180_animation; }
        if (lastDirection.y == 1 && movementVector.x == -1) { spriteEnum = Map::spider_idle270_animation; }
        if (lastDirection.x == -1 && movementVector.y == -1) { spriteEnum = Map::spider_idle0_animation; }
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
        if (strcmp(activity, "idle") == 0) {
            if (animCol >= 6) {
                animRow++;
                animCol = 0;
            }
            if (animRow >= 4) {
                animRow = 0;
            }
        }
        else {
            if (animCol >= 4) {
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
