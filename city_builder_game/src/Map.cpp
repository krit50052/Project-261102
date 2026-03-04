#include "Map.h"
#include <cmath>
#include <iostream>

const float Map::NODE_RADIUS = 30.0f;
const float Map::CONNECTION_THICKNESS = 3.0f;

Map::Map() {}
Map::~Map() {}

void Map::initialize() {
    m_nodes.clear();
    
    m_nodes.push_back(MapNode(1, "Capital", sf::Vector2f(200, 200), Owner::Player));
    m_nodes.back().setBuilding(BuildingType::Castle);
    
    m_nodes.push_back(MapNode(2, "Farmlands", sf::Vector2f(400, 150), Owner::Player));
    m_nodes.back().setBuilding(BuildingType::Farm);
    
    m_nodes.push_back(MapNode(3, "Gold Mine", sf::Vector2f(200, 400), Owner::Player));
    m_nodes.back().setBuilding(BuildingType::Mine);
    
    m_nodes.push_back(MapNode(4, "Plains", sf::Vector2f(640, 300), Owner::Neutral));
    m_nodes.push_back(MapNode(5, "Forest", sf::Vector2f(800, 150), Owner::Neutral));
    
    m_nodes.push_back(MapNode(6, "Enemy Camp", sf::Vector2f(1000, 300), Owner::Enemy));
    m_nodes.back().setBuilding(BuildingType::Barracks);
    
    m_nodes.push_back(MapNode(7, "Enemy Fort", sf::Vector2f(1080, 450), Owner::Enemy));
    m_nodes.back().setBuilding(BuildingType::Castle);

    m_nodes[0].addConnection(2);
    m_nodes[0].addConnection(3);
    m_nodes[1].addConnection(1);
    m_nodes[1].addConnection(4);
    m_nodes[1].addConnection(5);
    m_nodes[2].addConnection(1);
    m_nodes[2].addConnection(4);
    m_nodes[3].addConnection(2);
    m_nodes[3].addConnection(3);
    m_nodes[3].addConnection(5);
    m_nodes[3].addConnection(6);
    m_nodes[4].addConnection(2);
    m_nodes[4].addConnection(4);
    m_nodes[4].addConnection(7);
    m_nodes[5].addConnection(4);
    m_nodes[5].addConnection(7);
    m_nodes[6].addConnection(5);
    m_nodes[6].addConnection(6);

    std::cout << "Map initialized with " << m_nodes.size() << " nodes." << std::endl;
}

void Map::render(sf::RenderWindow& window) {
    renderConnections(window);
    renderNodes(window);
}

void Map::renderConnections(sf::RenderWindow& window) {
    sf::Color connectionColor(100, 100, 120, 150);

    for (const auto& node : m_nodes) {
        sf::Vector2f startPos = node.getPosition();
        
        for (int connectedId : node.getConnections()) {
            MapNode* targetNode = getNodeById(connectedId);
            if (targetNode && node.getId() < connectedId) {
                sf::Vector2f endPos = targetNode->getPosition();
                sf::Vector2f direction = endPos - startPos;
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                
                sf::RectangleShape line(sf::Vector2f(length, CONNECTION_THICKNESS));
                line.setPosition(startPos);
                line.setFillColor(connectionColor);
                
                float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
                line.setRotation(sf::degrees(angle));
                
                window.draw(line);
            }
        }
    }
}

void Map::renderNodes(sf::RenderWindow& window) {
    for (const auto& node : m_nodes) {
        sf::CircleShape circle(NODE_RADIUS);
        circle.setPosition(sf::Vector2f(node.getPosition().x - NODE_RADIUS, node.getPosition().y - NODE_RADIUS));
        circle.setFillColor(getOwnerColor(node.getOwner()));
        circle.setOutlineThickness(3.0f);
        circle.setOutlineColor(sf::Color(30, 30, 40));
        window.draw(circle);
        
        if (node.getBuilding() != BuildingType::None) {
            sf::CircleShape buildingIndicator(8.0f);
            buildingIndicator.setPosition(sf::Vector2f(node.getPosition().x + NODE_RADIUS - 16, 
                             node.getPosition().y - NODE_RADIUS));
            buildingIndicator.setFillColor(sf::Color(255, 215, 0));
            window.draw(buildingIndicator);
        }
    }
}

sf::Color Map::getOwnerColor(Owner owner) {
    switch (owner) {
        case Owner::Player: return sf::Color(60, 120, 216);
        case Owner::Enemy: return sf::Color(220, 60, 60);
        case Owner::Neutral: return sf::Color(160, 160, 170);
        default: return sf::Color::White;
    }
}

MapNode* Map::getNodeById(int id) {
    for (auto& node : m_nodes) {
        if (node.getId() == id) return &node;
    }
    return nullptr;
}

MapNode* Map::getNodeAtPosition(sf::Vector2f position) {
    for (auto& node : m_nodes) {
        sf::Vector2f nodePos = node.getPosition();
        float dx = position.x - nodePos.x;
        float dy = position.y - nodePos.y;
        float distanceSquared = dx * dx + dy * dy;
        
        if (distanceSquared <= NODE_RADIUS * NODE_RADIUS) {
            return &node;
        }
    }
    return nullptr;
}