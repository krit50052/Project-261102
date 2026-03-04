#pragma once

#include "GameResources.h"
#include "MapNode.h"
#include "Unit.h"
#include <SFML/Graphics.hpp>
#include <string>


class UIRenderer {
public:
    UIRenderer();
    ~UIRenderer();
    void renderNodeInfo(sf::RenderWindow& window, MapNode* node);
    bool renderEndTurnButton(sf::RenderWindow& window, sf::Vector2i mousePos, bool mouseClicked);
    bool loadFont(const std::string& fontPath);
    void renderTopBar(sf::RenderWindow& window, const GameResources& resources, int turnNumber);
    BuildingType renderBuildMenu(sf::RenderWindow& window, MapNode* node, const GameResources& resources, sf::Vector2i mousePos, bool mouseClicked);
    bool renderCaptureButton(sf::RenderWindow& window, MapNode* node, const GameResources& resources, sf::Vector2i mousePos, bool mouseClicked);
    UnitType renderRecruitMenu(sf::RenderWindow& window, MapNode* node, const GameResources& resources, sf::Vector2i mousePos, bool mouseClicked);
    int renderMoveMenu(sf::RenderWindow& window, MapNode* currentNode, const std::vector<int>& destinations, const std::vector<std::string>& destinationNames, sf::Vector2i mousePos, bool mouseClicked);
private:
    sf::Font m_font;
    bool m_fontLoaded;
    
    void drawResourceBar(sf::RenderWindow& window, int x, int y, const std::string& label, 
                        int value, sf::Color color);
};