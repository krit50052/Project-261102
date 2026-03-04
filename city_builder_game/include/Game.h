#pragma once
#include "Map.h"
#include "UIRenderer.h"
#include "Unit.h"
#include "GameResources.h"
#include <SFML/Graphics.hpp>
#include <memory>

class Game {
public:
    Game();
    ~Game();
    void captureTerritory(MapNode* node);
    bool isAdjacentToPlayer(MapNode* node);
    // Main game loop
    void run();

private:
    // Core methods
    void processEvents();
    void update(float deltaTime);
    void render();
    void endTurn();
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

    UIRenderer m_uiRenderer;
    GameResources m_resources;
    int m_turnNumber;

    MapNode* m_selectedNode;
    sf::Vector2i m_mousePosition;

    bool m_mouseClickedThisFrame;
    
    void buildBuilding(MapNode* node, BuildingType building);
    
    std::vector<Unit> m_units;
int m_nextUnitId;
Unit* m_selectedUnit;

void recruitUnit(MapNode* node, UnitType type);

void moveUnit(Unit* unit, int destinationNodeId);
void processCombat(int nodeId);
};

