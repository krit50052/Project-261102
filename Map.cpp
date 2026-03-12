#include "Map.h"
#include <cmath>
#include <iostream>

const float Map::NODE_RADIUS = 30.0f;           // Circle radius for nodes (pixels)
const float Map::CONNECTION_THICKNESS = 3.0f;   // Line thickness for connections (pixels)

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
    
    m_nodes.push_back(MapNode(4, "Empty Land", sf::Vector2f(400, 350), Owner::Player));
    
    m_nodes.push_back(MapNode(5, "Plains", sf::Vector2f(640, 300), Owner::Neutral));
    
    m_nodes.push_back(MapNode(6, "Forest", sf::Vector2f(800, 150), Owner::Neutral));
    
    m_nodes.push_back(MapNode(7, "Enemy Camp", sf::Vector2f(950, 300), Owner::Enemy));
    m_nodes.back().setBuilding(BuildingType::Barracks);
    
    m_nodes.push_back(MapNode(8, "Enemy Fort", sf::Vector2f(950, 450), Owner::Enemy));
    m_nodes.back().setBuilding(BuildingType::Castle);
    
    m_nodes[0].addConnection(2);  
    m_nodes[0].addConnection(3);  
    
    m_nodes[1].addConnection(1);  
    m_nodes[1].addConnection(4);  
    m_nodes[1].addConnection(5);  
    
   
    m_nodes[2].addConnection(1);  
    m_nodes[2].addConnection(4);  
    
    
    m_nodes[3].addConnection(2);  // Empty Land <-> Farmlands
    m_nodes[3].addConnection(3);  // Empty Land <-> Gold Mine
    m_nodes[3].addConnection(5); 
    
    // Plains connections (node 5, index 4) - Central hub
    m_nodes[4].addConnection(2);  // Plains <-> Farmlands
    m_nodes[4].addConnection(4);  // Plains <-> Empty Land
    m_nodes[4].addConnection(6);  // Plains <-> Forest
    m_nodes[4].addConnection(7);  // Plains <-> Enemy Camp
    
    // Forest connections (node 6, index 5)
    m_nodes[5].addConnection(5);  // Forest <-> Plains
    m_nodes[5].addConnection(7);  // Forest <-> Enemy Camp
    m_nodes[5].addConnection(8);  // Forest <-> Enemy Fort
    
    // Enemy Camp connections (node 7, index 6)
    m_nodes[6].addConnection(5);  // Enemy Camp <-> Plains
    m_nodes[6].addConnection(6);  // Enemy Camp <-> Forest
    m_nodes[6].addConnection(8);  // Enemy Camp <-> Enemy Fort
    
    // Enemy Fort connections (node 8, index 7)
    m_nodes[7].addConnection(6);  // Enemy Fort <-> Forest
    m_nodes[7].addConnection(7);  // Enemy Fort <-> Enemy Camp

    std::cout << "Map initialized with " << m_nodes.size() << " nodes." << std::endl;
}


void Map::render(sf::RenderWindow& window) {
    renderConnections(window);  // Draw first (background)
    renderNodes(window);        // Draw on top
}

void Map::renderConnections(sf::RenderWindow& window) {
    // Semi-transparent gray for subtle background lines
    sf::Color connectionColor(100, 100, 120, 150);

    for (const auto& node : m_nodes) {
        sf::Vector2f startPos = node.getPosition();
        
        // Draw line to each connected node
        for (int connectedId : node.getConnections()) {
            MapNode* targetNode = getNodeById(connectedId);
            
            // Only draw each connection once (avoid duplicates)
            if (targetNode && node.getId() < connectedId) {
                sf::Vector2f endPos = targetNode->getPosition();
                
                // Calculate line vector
                sf::Vector2f direction = endPos - startPos;
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                
                // Create line as a rotated rectangle
                sf::RectangleShape line(sf::Vector2f(length, CONNECTION_THICKNESS));
                line.setPosition(startPos);
                line.setFillColor(connectionColor);
                
                // Rotate to point toward target
                float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159265f;
                line.setRotation(sf::degrees(angle));
                
                window.draw(line);
            }
        }
    }
}

void Map::renderNodes(sf::RenderWindow& window) {
    for (const auto& node : m_nodes) {
        // Main node circle
        sf::CircleShape circle(NODE_RADIUS);
        circle.setPosition(sf::Vector2f(
            node.getPosition().x - NODE_RADIUS, 
            node.getPosition().y - NODE_RADIUS
        ));
        circle.setFillColor(getOwnerColor(node.getOwner()));
        circle.setOutlineThickness(3.0f);
        circle.setOutlineColor(sf::Color(30, 30, 40)); // Dark outline
        window.draw(circle);
        
        // Building indicator (small gold dot in top-right)
        if (node.getBuilding() != BuildingType::None) {
            sf::CircleShape buildingIndicator(8.0f);
            buildingIndicator.setPosition(sf::Vector2f(
                node.getPosition().x + NODE_RADIUS - 16, 
                node.getPosition().y - NODE_RADIUS
            ));
            buildingIndicator.setFillColor(sf::Color(255, 215, 0)); // Gold
            window.draw(buildingIndicator);
        }
    }
}

void Map::renderUnits(sf::RenderWindow& window, const std::vector<Unit>& units) {
    for (const auto& unit : units) {
        // Don't draw units that are moving between nodes
        if (unit.isMoving()) continue;
        
        // Get node position
        MapNode* node = getNodeById(unit.getNodeId());
        if (!node) continue;
        
        sf::Vector2f nodePos = node->getPosition();
        
        // Unit indicator circle (positioned to the right of node)
        sf::CircleShape unitCircle(10);
        unitCircle.setPosition(sf::Vector2f(nodePos.x + 20, nodePos.y - 25));
        
        // Color based on owner
        if (unit.getOwner() == Owner::Player) {
            unitCircle.setFillColor(sf::Color(100, 150, 255)); // Light blue
        } else {
            unitCircle.setFillColor(sf::Color(255, 100, 100)); // Light red
        }
        
        unitCircle.setOutlineThickness(2);
        unitCircle.setOutlineColor(sf::Color::White);
        window.draw(unitCircle);
        
        // Note: Unit count could be rendered as text here in future
        // Would require passing font to this function
    }
}

sf::Color Map::getOwnerColor(Owner owner) {
    switch (owner) {
        case Owner::Player:
            return sf::Color(60, 120, 216);   
        case Owner::Enemy:
            return sf::Color(220, 60, 60);    
        case Owner::Neutral:
            return sf::Color(160, 160, 170);  
        default:
            return sf::Color::White;
    }
}

MapNode* Map::getNodeById(int id) {
    for (auto& node : m_nodes) {
        if (node.getId() == id) {
            return &node;
        }
    }
    return nullptr; // Not found
}

MapNode* Map::getNodeAtPosition(sf::Vector2f position) {
    for (auto& node : m_nodes) {
        sf::Vector2f nodePos = node.getPosition();
        
        // Calculate distance from click to node center
        float dx = position.x - nodePos.x;
        float dy = position.y - nodePos.y;
        float distanceSquared = dx * dx + dy * dy;
        
        // Check if click is within node radius (use squared distance to avoid sqrt)
        if (distanceSquared <= NODE_RADIUS * NODE_RADIUS) {
            return &node;
        }
    }
    return nullptr;
}