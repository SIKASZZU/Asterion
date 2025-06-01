#include "abilities.h"
#include "game.h"
#include <iostream>

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
        // Draw (convert to screen-space)
        int screen_x = static_cast<int>(it->x) - offset.x;
        int screen_y = static_cast<int>(it->y) - offset.y;
        
        if (it->type == ARROW) {
            float dx = std::cos(it->direction);
            float dy = std::sin(it->direction);

            int end_x = screen_x + dx * it->length;
            int end_y = screen_y + dy * it->length;

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red arrow

            float perp_x = -dy;
            float perp_y = dx;
            float arr_length = std::sqrt(perp_x * perp_x + perp_y * perp_y);
            perp_x /= arr_length;
            perp_y /= arr_length;

            for (int i = -6; i <= 6; ++i) {
                int arr_offset_x = static_cast<int>(perp_x * i);
                int arr_offset_y = static_cast<int>(perp_y * i);

                SDL_RenderDrawLine(renderer,
                    screen_x + arr_offset_x, screen_y + arr_offset_y,
                    end_x + arr_offset_x, end_y + arr_offset_y);
            }

        } else {
            // Melee hitbox rectangle
            SDL_SetRenderDrawColor(renderer, 200, 0, 255, 255);  // Purple melee
            SDL_Rect r = { screen_x - 30, screen_y - 30, 60, 60 };
            SDL_RenderFillRect(renderer, &r);
        }

        ++it;  // it on item selles active_abilites vectoris
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
    a.length = 50;
    a.damage = 8;
    a.speed = 5;
    a.spawn_time = SDL_GetTicks();
    active_abilities.push_back(a);
}