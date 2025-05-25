#include "abilities.h"
#include "game.h"

std::vector<Ability> active_abilities;

void update_abilities(SDL_Renderer* renderer, struct Offset& offset) {
    for (auto it = active_abilities.begin(); it != active_abilities.end();) {
        if (it->type == ARROW) {
            it->x += std::cos(it->direction) * it->speed;
            it->y += std::sin(it->direction) * it->speed;
        }

        // Lifespan check
        if (SDL_GetTicks() - it->spawn_time > 1000) {
            it = active_abilities.erase(it);
            continue;
        }

        // Drawing with offset
        int screen_x = it->x - offset.x;
        int screen_y = it->y - offset.y;

        SDL_SetRenderDrawColor(renderer,
            it->type == ARROW ? 255 : 200,
            0,
            it->type == MELEE ? 255 : 0,
            255);
            
        SDL_Rect r = { screen_x - 5, screen_y - 5, 
            it->type == ARROW ? 30 : 60,  
            it->type == ARROW ? 30 : 60};

        SDL_RenderFillRect(renderer, &r);

        ++it;
    }
}


void use_melee(int x, int y, double direction) {
    Ability a;
    a.type = MELEE;
    a.x = static_cast<float>(x);
    a.y = static_cast<float>(y);
    a.direction = direction;
    a.range = 40;
    a.damage = 10;
    a.speed = 0; // instant
    a.spawn_time = SDL_GetTicks();
    active_abilities.push_back(a);
}


void use_arrow(int x, int y, double direction) {
    Ability a;
    a.type = ARROW;
    a.x = static_cast<float>(x);
    a.y = static_cast<float>(y);
    a.direction = direction;
    a.length = 200;
    a.damage = 8;
    a.speed = 5;
    a.spawn_time = SDL_GetTicks();
    active_abilities.push_back(a);
}