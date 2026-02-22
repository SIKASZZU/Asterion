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
int AnimPlayer::animCol = 0;
int AnimPlayer::animRow = 0;

SDL_FPoint AnimPlayer::lastDirection = { 0.f, 0.f };
int AnimPlayer::spriteEnum = 0;
SDL_FRect AnimPlayer::lastSrcRect = { 0,0,0,0 };

void animation_player(SDL_Renderer* renderer) {
    using namespace AnimPlayer;
    bool isMoving = (player.movementSpeed != 0);
    if (!isMoving) {
        textureMap[spriteEnum].render(renderer, &lastSrcRect, &player.rect);
        return;
    }


    SDL_FPoint currentDir = isMoving ? player.movementVector : lastDirection;
    if (isMoving) {
        lastDirection = player.movementVector;
    }

    if (lastDirection.x == 0 && lastDirection.y == 0) {
        // isMoving seems broken, not reset fast enough?
        // spriteEnum = Map::player_girl_animation_down;
    }

    if (currentDir.x == 1 && currentDir.y == 0) { spriteEnum = Map::player_girl_animation_down_right; }
    else if (currentDir.x == -1 && currentDir.y == 0) { spriteEnum = Map::player_girl_animation_up_left; }
    else if (currentDir.y == 1 && currentDir.x == 0) { spriteEnum = Map::player_girl_animation_down_left; }
    else if (currentDir.y == -1 && currentDir.x == 0) { spriteEnum = Map::player_girl_animation_up_right; }

    else if (currentDir.y == -1 && currentDir.x == 1) { spriteEnum = Map::player_girl_animation_right; }
    else if (currentDir.x == 1 && currentDir.y == 1) { spriteEnum = Map::player_girl_animation_down; }
    else if (currentDir.y == 1 && currentDir.x == -1) { spriteEnum = Map::player_girl_animation_left; }
    else if (currentDir.x == -1 && currentDir.y == -1) { spriteEnum = Map::player_girl_animation_up; }

    float animationSpeed;

    // animRow = player.shifting ? 1 : 0;
    animationSpeed = 200;
    if (SDL_GetTicks() - lastUpdate > static_cast<Uint32>(animationSpeed)) {
        lastUpdate = SDL_GetTicks();
        animCol = (animCol + 1) % 4;
        if (animCol == 0) {
            animRow += 1;
        }
        if (animRow == 2 && animCol == 1) {
            animRow = 0;
            animCol = 0;
        }
    }
    
    
    SDL_FRect srcRect = {
        static_cast<float>(animCol * spriteWidth),
        static_cast<float>(animRow * spriteHeight),
        static_cast<float>(spriteWidth),
        static_cast<float>(spriteHeight)
    };

    lastSrcRect = srcRect;

    textureMap[spriteEnum].render(renderer, &srcRect, &player.rect);
}