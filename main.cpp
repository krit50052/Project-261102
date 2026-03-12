#include "Game.h"
#include <iostream>
#include <exception>

int main() {
    try {
        std::cout << "=== City Builder Game ===" << std::endl;
        std::cout << "Starting game..." << std::endl;
        
        Game game;
        game.run();
        
        std::cout << "Game ended normally." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
