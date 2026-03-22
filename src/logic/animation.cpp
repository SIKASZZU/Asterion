#include "player.hpp"
#include "enemy.hpp"
#include "animation.hpp"
#include "textures.hpp"
#include <cstring>
#include <vector>

// --------------- Enemy --------------- //

void Enemy::animation(SDL_Renderer* renderer) {
    using namespace AnimEnemy;

    int animationSpeed = 48;
    int maxCols = 4; int maxRows = 4;

    switch (state) {
    case EnemyState::Run: {
        lastDirection = movementVector;
        if (movementVector.x == 1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_run135_animation; }
        if (movementVector.x == -1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_run315_animation; }
        if (movementVector.y == 1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_run225_animation; }
        if (movementVector.y == -1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_run45_animation; }

        if (movementVector.y == -1 && movementVector.x == 1) { anim_spriteEnum = Map::spider_run90_animation; }
        if (movementVector.x == 1 && movementVector.y == 1) { anim_spriteEnum = Map::spider_run180_animation; }
        if (movementVector.y == 1 && movementVector.x == -1) { anim_spriteEnum = Map::spider_run270_animation; }
        if (movementVector.x == -1 && movementVector.y == -1) { anim_spriteEnum = Map::spider_run0_animation; }
        break;
    }
    case EnemyState::Walk: {
        lastDirection = movementVector;
        animationSpeed = 24;
        if (movementVector.x == 1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_walk135_animation; }
        if (movementVector.x == -1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_walk315_animation; }
        if (movementVector.y == 1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_walk225_animation; }
        if (movementVector.y == -1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_walk45_animation; }

        if (movementVector.y == -1 && movementVector.x == 1) { anim_spriteEnum = Map::spider_walk90_animation; }
        if (movementVector.x == 1 && movementVector.y == 1) { anim_spriteEnum = Map::spider_walk180_animation; }
        if (movementVector.y == 1 && movementVector.x == -1) { anim_spriteEnum = Map::spider_walk270_animation; }
        if (movementVector.x == -1 && movementVector.y == -1) { anim_spriteEnum = Map::spider_walk0_animation; }
        break;
    }
    case EnemyState::Idle: {
        animationSpeed = 60;
        maxCols = 6;
        if (lastDirection.x == 1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_idle135_animation; }
        if (lastDirection.x == -1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_idle315_animation; }
        if (lastDirection.y == 1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_idle225_animation; }
        if (lastDirection.y == -1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_idle45_animation; }

        if (lastDirection.y == -1 && movementVector.x == 1) { anim_spriteEnum = Map::spider_idle90_animation; }
        if (lastDirection.x == 1 && movementVector.y == 1) { anim_spriteEnum = Map::spider_idle180_animation; }
        if (lastDirection.y == 1 && movementVector.x == -1) { anim_spriteEnum = Map::spider_idle270_animation; }
        if (lastDirection.x == -1 && movementVector.y == -1) { anim_spriteEnum = Map::spider_idle0_animation; }
        break;
    }
    case EnemyState::Raise: {
        animationSpeed = 80;
        if (lastDirection.x == 1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_raise135_animation; }
        if (lastDirection.x == -1 && movementVector.y == 0) { anim_spriteEnum = Map::spider_raise315_animation; }
        if (lastDirection.y == 1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_raise225_animation; }
        if (lastDirection.y == -1 && movementVector.x == 0) { anim_spriteEnum = Map::spider_raise45_animation; }

        if (lastDirection.y == -1 && movementVector.x == 1) { anim_spriteEnum = Map::spider_raise90_animation; }
        if (lastDirection.x == 1 && movementVector.y == 1) { anim_spriteEnum = Map::spider_raise180_animation; }
        if (lastDirection.y == 1 && movementVector.x == -1) { anim_spriteEnum = Map::spider_raise270_animation; }
        if (lastDirection.x == -1 && movementVector.y == -1) { anim_spriteEnum = Map::spider_raise0_animation; }
        break;
    }
    default: {
        std::cout << "Enemy state is unknown. No spriteEnum for enemy" << '\n';
        break;
    }
    }

    SDL_FRect dstRect = rect;
    float scaleFactor = 1.0f;
    dstRect.w = spriteWidth * scaleFactor;
    dstRect.h = spriteHeight * scaleFactor;
    // center inside the original rect (e.g. tile)
    dstRect.x = rect.x + (rect.w - dstRect.w) * 0.5f;
    dstRect.y = rect.y + (rect.h - dstRect.h) * 0.5f;

    bool updateFrame = (SDL_GetTicks() - anim_lastUpdate > animationSpeed);
    if (anim_previousState != static_cast<std::underlying_type_t<EnemyActivity>>(activity)) {
        updateFrame = true;
        anim_currentAnimRow = 0; anim_currentAnimCol = 0;
    }
    anim_previousState = static_cast<std::underlying_type_t<EnemyActivity>>(activity);

    if (updateFrame) {
        anim_lastUpdate = SDL_GetTicks();
        anim_currentAnimCol = (anim_currentAnimCol + 1) % maxCols;

        if (anim_currentAnimCol == 0) {
            anim_currentAnimRow += 1;
        }
        if (anim_currentAnimRow >= maxRows) {
            anim_currentAnimRow = 0; anim_currentAnimCol = 0;
            if (spawning) spawning = false;
        }

    }
    SDL_FRect srcRect = {
        static_cast<float>(anim_currentAnimCol * spriteWidth),
        static_cast<float>(anim_currentAnimRow * spriteHeight),
        static_cast<float>(spriteWidth),
        static_cast<float>(spriteHeight)
    };

    textureMap[anim_spriteEnum].render(renderer, &srcRect, &dstRect);
}

// --------------- Player --------------- //

Uint32 AnimPlayer::lastUpdate = SDL_GetTicks();
int AnimPlayer::currentAnimCol = 0;
int AnimPlayer::currentAnimRow = 0;
int AnimPlayer::previousState = 0;
bool AnimPlayer::singleLoop = false;
bool AnimPlayer::loopComplete = false;


SDL_Point AnimPlayer::lastDirectionVector = { 0, 0 };
int AnimPlayer::spriteEnum = 0;

int animation_player_idle(SDL_Point movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_idle_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_idle_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_idle_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_idle_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_idle_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_idle_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_idle_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_idle_up; }
    return Map::ERROR_CUBE;
}

int animation_player_walk(SDL_Point movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_walk_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_walk_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_walk_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_walk_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_walk_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_walk_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_walk_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_walk_up; }
    return Map::ERROR_CUBE;
}

int animation_player_run(SDL_Point movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_run_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_run_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_run_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_run_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_run_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_run_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_run_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_run_up; }
    return Map::ERROR_CUBE;
}

int animation_player_jump(SDL_Point movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_jump_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_jump_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_jump_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_jump_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_jump_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_jump_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_jump_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_jump_up; }
    return Map::ERROR_CUBE;
}

int animation_player_running_jump(SDL_Point movementVector) {
    if (movementVector.x == 1 && movementVector.y == 0) { return Map::player_girl_running_jump_down_right; }
    else if (movementVector.x == -1 && movementVector.y == 0) { return Map::player_girl_running_jump_up_left; }
    else if (movementVector.y == 1 && movementVector.x == 0) { return Map::player_girl_running_jump_down_left; }
    else if (movementVector.y == -1 && movementVector.x == 0) { return Map::player_girl_running_jump_up_right; }

    else if (movementVector.y == -1 && movementVector.x == 1) { return Map::player_girl_running_jump_right; }
    else if (movementVector.x == 1 && movementVector.y == 1) { return Map::player_girl_running_jump_down; }
    else if (movementVector.y == 1 && movementVector.x == -1) { return Map::player_girl_running_jump_left; }
    else if (movementVector.x == -1 && movementVector.y == -1) { return Map::player_girl_running_jump_up; }
    return Map::ERROR_CUBE;
}

void animation_player(SDL_Renderer* renderer) {
    using namespace AnimPlayer;
    singleLoop = false;

    // detect et player enam ei liigu
    if ((player.movementVector.x != 0 || player.movementVector.y != 0)) {
        lastDirectionVector = player.movementVector;
    }
    else if (player.state != PlayerState::Idle) {
        player.movementVector = lastDirectionVector;
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
    case PlayerState::Idle: {
        spriteEnum = animation_player_idle(lastDirectionVector);
        maxRows = 3; lastRowCols = 1;
        break;
    }
    case PlayerState::Jump: {
        spriteEnum = animation_player_jump(lastDirectionVector);
        maxRows = 2; lastRowCols = 2;
        singleLoop = true;
        break;
    }
    case PlayerState::RunningJump: {
        spriteEnum = animation_player_running_jump(lastDirectionVector);
        maxRows = 1; lastRowCols = 3;
        singleLoop = true;
        break;
    }
    default: {
        std::cout << "Player state is unknown. No spriteEnum for player." << '\n';
        break;
    }
    }

    bool updateFrame = SDL_GetTicks() - lastUpdate > static_cast<Uint32>(player.animationSpeed);
    if (previousState != static_cast<std::underlying_type_t<PlayerState>>(player.state)) {
        updateFrame = true;
        currentAnimRow = 0; currentAnimCol = 0;
        loopComplete = false;
    }
    previousState = static_cast<std::underlying_type_t<PlayerState>>(player.state);

    if (singleLoop == true && loopComplete == true) {
        // use the animation's last frame on repeat.
        currentAnimCol = lastRowCols;
        currentAnimRow = maxRows;
    }

    SDL_FRect srcRect = {
        static_cast<float>(currentAnimCol * spriteWidth),
        static_cast<float>(currentAnimRow * spriteHeight),
        static_cast<float>(spriteWidth),
        static_cast<float>(spriteHeight)
    };

    textureMap[spriteEnum].render(renderer, &srcRect, &player.rect);

    // currentAnimCol, Row for next frame.
    // rendering before this means, it can render the 0,0 index perfect.
    if (updateFrame) {
        lastUpdate = SDL_GetTicks();

        currentAnimCol = (currentAnimCol + 1) % 4;
        // std::cout << "x" << currentAnimRow << ' ' << currentAnimCol << " " << "max: " << maxRows << " " << lastRowCols << '\n';

        if (currentAnimCol == 0) {
            currentAnimRow += 1;
        }
        if (currentAnimRow >= maxRows && currentAnimCol >= lastRowCols) {
            currentAnimRow = 0; currentAnimCol = 0;
            loopComplete = true;
            // std::cout << "max col, row reached!" << '\n';
        }
    }
}