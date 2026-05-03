#define SDL_MAIN_HANDLED
#include "game.hpp"
#include <iostream>

int main() {
    std::cout << "C++ version: " << __cplusplus << '\n';

    if (!Game::init())    return 1;
    Game::load_level();

    while (Game::is_running()) {
        Game::handle_events();
        Game::update();
        Game::render();
    }

    Game::shutdown();
    return 0;
}