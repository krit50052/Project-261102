#pragma once
#include "Map.h"
#include <SFML/Graphics.hpp>
#include <memory>

class Game {
public:
    Game();
    ~Game();

    // Main game loop
    void run();

private:
    // Core methods
    void processEvents();
    void update(float deltaTime);
    void render();

    // Window
    sf::RenderWindow m_window;
    
    // Game state
    bool m_isRunning;
    
    // Frame time management
    sf::Clock m_clock;
    
    // Window settings
    static const int WINDOW_WIDTH = 1280;
    static const int WINDOW_HEIGHT = 720;
    static constexpr float FPS = 60.0f;
    Map m_map;
};
