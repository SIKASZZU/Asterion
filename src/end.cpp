#include <iostream>
#include <algorithm>
// #include <SDL3/SDL.h>

#include "end.hpp"
#include "player.hpp"
#include "map.hpp"

namespace Portal {
    bool has_entered() {
        std::pair<int, int> player_grid = {
            static_cast<int>((player.x + (player.size / 2)) / tileSize),
            static_cast<int>((player.y + (player.size / 2)) / tileSize)
        };

        if (map[player_grid.second][player_grid.first] == Map::VOID_CUBE
            || map[player_grid.second][player_grid.first] == VOID_CUBE_NEIGHBOUR) {
            std::cout << "Player entered the portal!" << '\n';
            Ending::start = true;
            return true;
        }

        return false;
    }
}

namespace Ending {
    bool start = false;

    void update(SDL_Renderer* renderer) {

        SDL_RenderClear(renderer);
        std::cout << "Render cleared & DrawColor = 0,0,0" << '\n';
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}