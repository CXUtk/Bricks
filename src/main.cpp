#include <glm/glm.hpp>
#include <iostream>

#include "Game.h"

int main(int argc, char** argv) {
    try {
        Game& game = Game::GetInstance();
        game.init();
        game.run();
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
    int x;
    std::cin >> x;
}
