#pragma once
#include "MapNode.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Map {
public:
    Map();
    ~Map();
    void initialize();
    void render(sf::RenderWindow& window);
    MapNode* getNodeById(int id);
    MapNode* getNodeAtPosition(sf::Vector2f position);

private:
    std::vector<MapNode> m_nodes;
    sf::Color getOwnerColor(Owner owner);
    void renderConnections(sf::RenderWindow& window);
    void renderNodes(sf::RenderWindow& window);
    static const float NODE_RADIUS;
    static const float CONNECTION_THICKNESS;
};