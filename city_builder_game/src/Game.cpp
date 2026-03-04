#include "Game.h"
#include <iostream>

Game::Game() 
    : m_window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "City Builder - Demacia Rising Style")
    , m_isRunning(true)
{
    // Set frame rate limit

    m_window.setFramerateLimit(static_cast<unsigned int>(FPS));
    m_map.initialize();
    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "Window size: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
}

Game::~Game() {
    std::cout << "Game shutting down..." << std::endl;
}

void Game::run() {
    while (m_isRunning && m_window.isOpen()) {
        // Get delta time
        float deltaTime = m_clock.restart().asSeconds();
        
        // Game loop
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_isRunning = false;
            m_window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_isRunning = false;
                m_window.close();
            }
        }
    }
}

void Game::update(float deltaTime) {
    // Game logic will go here
    // For now, just a placeholder
}

void Game::render() {
    // Clear window with a dark blue color (sky-like)
    m_window.clear(sf::Color(30, 40, 80));
    m_map.render(m_window);
    // Rendering will go here
    
    // Display
    m_window.display();
}