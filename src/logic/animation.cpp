#include "player.hpp"
#include "enemy.hpp"
#include "animation.hpp"
#include "textures.hpp"
#include <cstring>
#include <vector>

// --------------- Enemy --------------- //

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

// --------------- Player --------------- //

Uint32 AnimPlayer::lastUpdate = SDL_GetTicks();
int AnimPlayer::currentAnimCol = 0;
int AnimPlayer::currentAnimRow = 0;
int AnimPlayer::previousState = 0;

SDL_FPoint AnimPlayer::lastDirectionVector = { 0.f, 0.f };
int AnimPlayer::spriteEnum = 0;

int animation_player_idle(SDL_FPoint movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_idle_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_idle_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_idle_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_idle_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_idle_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_idle_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_idle_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_idle_up; }
    return Map::player_girl_idle_down;
}

int animation_player_walk(SDL_FPoint movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_walk_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_walk_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_walk_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_walk_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_walk_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_walk_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_walk_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_walk_up; }
    return Map::player_girl_walk_down;
}

int animation_player_run(SDL_FPoint movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_run_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_run_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_run_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_run_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_run_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_run_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_run_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_run_up; }
    return Map::player_girl_run_down;
}

void animation_player(SDL_Renderer* renderer) {
    using namespace AnimPlayer;

    if ((player.movementVector.x != 0 || player.movementVector.y != 0)) {
        lastDirectionVector = player.movementVector;
    }

    int maxRows, lastRowCols;
    switch (player.state) {
    case PlayerState::Walk: {
        spriteEnum = animation_player_walk(player.movementVector);
        maxRows = 2; lastRowCols = 1;
        break;
    }
    case PlayerState::Run: {
        spriteEnum = animation_player_run(player.movementVector);
        maxRows = 1; lastRowCols = 1;
        break;
    }
    default: {
        spriteEnum = animation_player_idle(lastDirectionVector);
        maxRows = 3; lastRowCols = 1;
        break;
    }
    }

    float animationSpeed = player.shifting ? tileSize * 1.4f : tileSize * 0.8f;
    bool updateFrame = SDL_GetTicks() - lastUpdate > static_cast<Uint32>(animationSpeed);
    if (previousState != static_cast<std::underlying_type_t<PlayerState>>(player.state)) {
        updateFrame = true;
    }
    previousState = static_cast<std::underlying_type_t<PlayerState>>(player.state);

    if (updateFrame) {
        lastUpdate = SDL_GetTicks();
        currentAnimCol = (currentAnimCol + 1) % 4;

        // std::cout << "x" << currentAnimRow << ' ' << currentAnimCol << '\n';

        if (currentAnimCol == 0) {
            currentAnimRow += 1;
        }
        if (currentAnimRow >= maxRows && currentAnimCol >= lastRowCols) {
            currentAnimRow = 0; currentAnimCol = 0;
        }

    }

    SDL_FRect srcRect = {
        static_cast<float>(currentAnimCol * spriteWidth),
        static_cast<float>(currentAnimRow * spriteHeight),
        static_cast<float>(spriteWidth),
        static_cast<float>(spriteHeight)
    };

    textureMap[spriteEnum].render(renderer, &srcRect, &player.rect);
}