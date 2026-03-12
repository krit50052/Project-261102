#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Map.h"
#include "UIRenderer.h"
#include "GameResources.h"
#include "Unit.h"

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    
    void update(float deltaTime);

    void render();

    void endTurn();

    void checkVictoryConditions();

    void buildBuilding(MapNode* node, BuildingType building);

    void captureTerritory(MapNode* node);

    bool isAdjacentToPlayer(MapNode* node);

    void recruitUnit(MapNode* node, UnitType type);

    void moveUnit(Unit* unit, int destinationNodeId);
 
    void processCombat(int nodeId);

    void processEnemyTurn();
    

    void enemyCaptureNeutral();

    void enemyRecruitUnits();
    

    void enemyMoveUnits();

    sf::RenderWindow m_window;      
    sf::Clock m_clock;                 
    bool m_isRunning;                
    
 
    Map m_map;                      
    UIRenderer m_uiRenderer;         
    GameResources m_resources;         
    std::vector<Unit> m_units;        
    

    int m_turnNumber;                   
    int m_nextUnitId;                 
    bool m_gameOver;                  
    std::string m_gameResult;           
    
  
    MapNode* m_selectedNode;          
    Unit* m_selectedUnit;             
    sf::Vector2i m_mousePosition;      
    bool m_mouseClickedThisFrame;       
    

    static const int WINDOW_WIDTH = 1280;
    static const int WINDOW_HEIGHT = 720;
    static constexpr float FPS = 60.0f;
};