#define SDL_MAIN_HANDLED
#include "game.hpp"
#include "game_state.hpp"
#include <iostream>

int main() {

    GameState* gS = new GameState();

    if (!Game::init(gS)) {
        delete gS;
        return 1;
    }

    Game::load_level(gS);

    while (Game::is_running(gS)) {
        Game::handle_events(gS);
        Game::update(gS);
        Game::render(gS);
    }

    Game::shutdown(gS);

    delete gS;
    return 0;
}