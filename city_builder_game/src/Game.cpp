#include "Game.h"
#include <set>
#include <algorithm>
#include <iostream>

Game::Game() 
    : m_window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "City Builder - Demacia Rising Style")
    , m_isRunning(true)
{
    // Set frame rate limit

    m_window.setFramerateLimit(static_cast<unsigned int>(FPS));
    m_map.initialize();
// Debug: Print all node positions
    std::cout << "\n=== Node Positions ===" << std::endl;
    for (const auto& node : m_map.getNodes()) {
        std::cout << "Node " << node.getId() << " (" << node.getName() << "): " 
                  << node.getPosition().x << ", " << node.getPosition().y 
                  << " Owner: " << static_cast<int>(node.getOwner()) << std::endl;
    }
    std::cout << "=====================\n" << std::endl;
    m_turnNumber = 1;
    m_uiRenderer.loadFont("C:/Windows/Fonts/arial.ttf");
    m_selectedNode = nullptr;
    m_nextUnitId = 1;
    m_selectedUnit = nullptr;

// Add some starting enemy units
m_units.push_back(Unit(m_nextUnitId++, UnitType::Infantry, 5, Owner::Enemy, 7)); // Enemy Camp (was 6)
m_units.push_back(Unit(m_nextUnitId++, UnitType::Rangers, 3, Owner::Enemy, 8));  // Enemy Fort (was 7)
    m_mouseClickedThisFrame = false;
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
        else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                m_mouseClickedThisFrame = true;
                
                // Convert mouse position to world coordinates
                sf::Vector2f mousePos(static_cast<float>(mouseButtonPressed->position.x),
                                     static_cast<float>(mouseButtonPressed->position.y));
                
                // Don't select nodes if clicking in menu panel area
                bool inMenuArea = (mousePos.x >= 970 && mousePos.y >= 140 && mousePos.y <= 610);
                
                if (!inMenuArea) {
                    // Check if clicked on a node
                    MapNode* clickedNode = m_map.getNodeAtPosition(mousePos);
                    
                    if (clickedNode) {
                        m_selectedNode = clickedNode;
                        
                        // Select player units at this node
                        m_selectedUnit = nullptr;
                        for (auto& unit : m_units) {
                            if (unit.getNodeId() == clickedNode->getId() && 
                                unit.getOwner() == Owner::Player && 
                                !unit.isMoving()) {
                                m_selectedUnit = &unit;
                                break;
                            }
                        }
                        
                        std::cout << "Selected: " << clickedNode->getName() 
                                  << " (Owner: " << (clickedNode->getOwner() == Owner::Player ? "Player" : 
                                                     clickedNode->getOwner() == Owner::Enemy ? "Enemy" : "Neutral") 
                                  << ")" << std::endl;
                    } else {
                        // Only deselect if clicking on empty space, not UI
                        m_selectedNode = nullptr;
                        m_selectedUnit = nullptr;
                    }
                }
            }
        }
    }
}

void Game::update(float deltaTime) {
    // Update mouse position
    m_mousePosition = sf::Mouse::getPosition(m_window);
}

void Game::render() {
    // Clear window with a dark blue color (sky-like)
    m_window.clear(sf::Color(30, 40, 80));
    m_map.render(m_window);
    m_map.renderUnits(m_window, m_units);
    
    // Highlight selected node
    if (m_selectedNode) {
        sf::CircleShape highlight(35.0f);
        sf::Vector2f nodePos = m_selectedNode->getPosition();
        highlight.setPosition(sf::Vector2f(nodePos.x - 35.0f, nodePos.y - 35.0f));
        highlight.setFillColor(sf::Color::Transparent);
        highlight.setOutlineThickness(4.0f);
        highlight.setOutlineColor(sf::Color(255, 215, 0)); // Gold outline
        m_window.draw(highlight);
    }
    
    m_uiRenderer.renderTopBar(m_window, m_resources, m_turnNumber);
    m_uiRenderer.renderNodeInfo(m_window, m_selectedNode);
    
    // Show appropriate menu based on priority
    bool menuShown = false;
    
    // Priority 1: Unit movement menu (if unit selected)
    if (!menuShown && m_selectedUnit && !m_selectedUnit->isMoving() && m_resources.actionPoints >= 1) {
        MapNode* currentNode = m_map.getNodeById(m_selectedUnit->getNodeId());
        if (currentNode) {
            std::vector<int> destinations;
            std::vector<std::string> destinationNames;
            
            for (int connId : currentNode->getConnections()) {
                MapNode* destNode = m_map.getNodeById(connId);
                if (destNode) {
                    destinations.push_back(connId);
                    destinationNames.push_back(destNode->getName());
                }
            }
            
            int selectedDest = m_uiRenderer.renderMoveMenu(m_window, currentNode, destinations, destinationNames, m_mousePosition, m_mouseClickedThisFrame);
            if (selectedDest != -1) {
                moveUnit(m_selectedUnit, selectedDest);
            }
            menuShown = true;
        }
    }
    
    // Priority 2 & 3: Building/Recruitment menus
    if (!menuShown && m_selectedNode && m_selectedNode->getOwner() == Owner::Player) {
        if (m_selectedNode->getBuilding() == BuildingType::Barracks) {
            // Recruit menu for barracks
            UnitType selectedUnitType = m_uiRenderer.renderRecruitMenu(m_window, m_selectedNode, m_resources, m_mousePosition, m_mouseClickedThisFrame);
            if (selectedUnitType != static_cast<UnitType>(-1)) {
                recruitUnit(m_selectedNode, selectedUnitType);
            }
            menuShown = true;
        } else if (m_selectedNode->getBuilding() == BuildingType::None) {
            // Build menu for empty player nodes
            BuildingType selectedBuilding = m_uiRenderer.renderBuildMenu(m_window, m_selectedNode, m_resources, m_mousePosition, m_mouseClickedThisFrame);
            if (selectedBuilding != BuildingType::None) {
                buildBuilding(m_selectedNode, selectedBuilding);
            }
            menuShown = true;
        }
    }
    
    // Priority 4: Capture menu
    if (!menuShown && m_selectedNode && m_selectedNode->getOwner() == Owner::Neutral) {
        if (isAdjacentToPlayer(m_selectedNode)) {
            if (m_uiRenderer.renderCaptureButton(m_window, m_selectedNode, m_resources, m_mousePosition, m_mouseClickedThisFrame)) {
                captureTerritory(m_selectedNode);
            }
            menuShown = true;
        }
    }
    
    // Render and check End Turn button
    if (m_uiRenderer.renderEndTurnButton(m_window, m_mousePosition, m_mouseClickedThisFrame)) {
        endTurn();
    }
    
    // Reset click flag
    m_mouseClickedThisFrame = false;
    
    // Display
    m_window.display();
}

void Game::endTurn() {
    std::cout << "\n=== End Turn " << m_turnNumber << " ===" << std::endl;
    
    // 1. Calculate resource production from buildings
    int goldIncome = 0;
    int foodIncome = 0;
    int woodIncome = 0;
    
    for (const auto& node : m_map.getNodes()) {
        if (node.getOwner() == Owner::Player) {
            node.getProduction(goldIncome, foodIncome, woodIncome);
        }
    }
    
    std::cout << "Production: +" << goldIncome << " Gold, +" 
              << foodIncome << " Food, +" << woodIncome << " Wood" << std::endl;
    
    // 2. Calculate upkeep (placeholder - no units yet, so 0)
    int upkeep = 0; // Will add unit upkeep later
    
    std::cout << "Upkeep: -" << upkeep << " Food" << std::endl;
    
    // 3. Update resources
    m_resources.gold += goldIncome;
    m_resources.food += foodIncome - upkeep;
    m_resources.wood += woodIncome;
    
    // Make sure resources don't go negative
    if (m_resources.food < 0) m_resources.food = 0;
    
    // 4. Reset action points
    m_resources.actionPoints = 3;
    
//  Process unit movement and combat
    for (auto& unit : m_units) {
        unit.updateMovement();
    }
    
    // Check for battles at each node
    std::set<int> processedNodes;
    for (const auto& unit : m_units) {
        if (!unit.isMoving() && processedNodes.find(unit.getNodeId()) == processedNodes.end()) {
            processCombat(unit.getNodeId());
            processedNodes.insert(unit.getNodeId());
        }
    }

    // 5. Increment turn
    m_turnNumber++;
    
    std::cout << "New resources: " << m_resources.gold << " Gold, " 
              << m_resources.food << " Food, " << m_resources.wood << " Wood" << std::endl;
    std::cout << "=== Turn " << m_turnNumber << " Started ===" << std::endl;
}


    void Game::buildBuilding(MapNode* node, BuildingType building) {
    if (!node || node->getOwner() != Owner::Player || node->getBuilding() != BuildingType::None) {
        std::cout << "Cannot build here!" << std::endl;
        return;
    }
    
    BuildingInfo info = getBuildingInfo(building);
    
    // Check resources
    if (!m_resources.canAfford(info.goldCost, 0, info.woodCost)) {
        std::cout << "Not enough resources!" << std::endl;
        return;
    }
    
    // Check action points
    if (m_resources.actionPoints < 1) {
        std::cout << "Not enough action points!" << std::endl;
        return;
    }
    
    // Spend resources
    m_resources.gold -= info.goldCost;
    m_resources.wood -= info.woodCost;
    m_resources.actionPoints -= 1;
    
    // Build!
    node->setBuilding(building);
    
    std::cout << "Built " << info.name << " at " << node->getName() << "!" << std::endl;
    std::cout << "Remaining: " << m_resources.gold << " Gold, " 
              << m_resources.wood << " Wood, " 
              << m_resources.actionPoints << " AP" << std::endl;
}

bool Game::isAdjacentToPlayer(MapNode* node) {
    if (!node) return false;
    
    // Check if any connected node is owned by player
    for (int connectedId : node->getConnections()) {
        MapNode* connectedNode = m_map.getNodeById(connectedId);
        if (connectedNode && connectedNode->getOwner() == Owner::Player) {
            return true;
        }
    }
    return false;
}

void Game::captureTerritory(MapNode* node) {
    if (!node || node->getOwner() != Owner::Neutral) {
        std::cout << "Cannot capture this territory!" << std::endl;
        return;
    }
    
    if (!isAdjacentToPlayer(node)) {
        std::cout << "Territory must be adjacent to your land!" << std::endl;
        return;
    }
    
    // Check resources
    if (m_resources.gold < 50) {
        std::cout << "Not enough gold! Need 50." << std::endl;
        return;
    }
    
    if (m_resources.actionPoints < 1) {
        std::cout << "Not enough action points!" << std::endl;
        return;
    }
    
    // Spend resources
    m_resources.gold -= 50;
    m_resources.actionPoints -= 1;
    
    // Capture!
    node->setOwner(Owner::Player);
    
    std::cout << "Captured " << node->getName() << "!" << std::endl;
    std::cout << "Remaining: " << m_resources.gold << " Gold, " 
              << m_resources.actionPoints << " AP" << std::endl;
}

void Game::recruitUnit(MapNode* node, UnitType type) {
    if (!node || node->getOwner() != Owner::Player || node->getBuilding() != BuildingType::Barracks) {
        std::cout << "Need a Barracks to recruit units!" << std::endl;
        return;
    }
    
    UnitStats stats = getUnitStats(type);
    
    if (!m_resources.canAfford(stats.goldCost, stats.foodCost, 0)) {
        std::cout << "Not enough resources!" << std::endl;
        return;
    }
    
    if (m_resources.actionPoints < 1) {
        std::cout << "Not enough action points!" << std::endl;
        return;
    }
    
    // Spend resources
    m_resources.gold -= stats.goldCost;
    m_resources.food -= stats.foodCost;
    m_resources.actionPoints -= 1;
    
    // Find existing unit stack or create new
    Unit* existingUnit = nullptr;
    for (auto& unit : m_units) {
        if (unit.getNodeId() == node->getId() && unit.getType() == type && unit.getOwner() == Owner::Player) {
            existingUnit = &unit;
            break;
        }
    }
    
    if (existingUnit) {
        existingUnit->setCount(existingUnit->getCount() + 3);
    } else {
        m_units.push_back(Unit(m_nextUnitId++, type, 3, Owner::Player, node->getId()));
    }
    
    std::cout << "Recruited 3 " << getUnitName(type) << " at " << node->getName() << "!" << std::endl;
}


void Game::moveUnit(Unit* unit, int destinationNodeId) {
    if (!unit || m_resources.actionPoints < 1) {
        std::cout << "Cannot move unit!" << std::endl;
        return;
    }
    
    MapNode* destNode = m_map.getNodeById(destinationNodeId);
    if (!destNode) return;
    
    m_resources.actionPoints -= 1;
    unit->startMoving(destinationNodeId, 1);
    
    std::cout << "Unit moving to " << destNode->getName() << "..." << std::endl;
    m_selectedUnit = nullptr;
}

void Game::processCombat(int nodeId) {
    // Find all units at this node
    std::vector<Unit*> playerUnits;
    std::vector<Unit*> enemyUnits;
    
    for (auto& unit : m_units) {
        if (unit.getNodeId() == nodeId && !unit.isMoving()) {
            if (unit.getOwner() == Owner::Player) {
                playerUnits.push_back(&unit);
            } else if (unit.getOwner() == Owner::Enemy) {
                enemyUnits.push_back(&unit);
            }
        }
    }
    
    if (playerUnits.empty() || enemyUnits.empty()) {
        return; // No battle
    }
    
    std::cout << "\n*** BATTLE at node " << nodeId << " ***" << std::endl;
    
    // Calculate total power
    int playerPower = 0;
    int enemyPower = 0;
    
    for (auto* unit : playerUnits) {
        UnitStats stats = getUnitStats(unit->getType());
        int power = unit->getCount() * stats.attack;
        
        // Check for counter bonus
        for (auto* enemy : enemyUnits) {
            if (stats.counters == enemy->getType()) {
                power = static_cast<int>(power * 1.3);
                std::cout << getUnitName(unit->getType()) << " gets counter bonus vs " << getUnitName(enemy->getType()) << "!" << std::endl;
            }
        }
        playerPower += power;
    }
    
    for (auto* unit : enemyUnits) {
        UnitStats stats = getUnitStats(unit->getType());
        int power = unit->getCount() * stats.defense;
        
        // Check for counter bonus
        for (auto* player : playerUnits) {
            if (stats.counters == player->getType()) {
                power = static_cast<int>(power * 1.5);
                std::cout << getUnitName(unit->getType()) << " gets counter bonus vs " << getUnitName(player->getType()) << "!" << std::endl;
            }
        }
        enemyPower += power;
    }
    
    std::cout << "Player Power: " << playerPower << " vs Enemy Power: " << enemyPower << std::endl;
    
    if (playerPower > enemyPower) {
        std::cout << ">>> VICTORY! <<<" << std::endl;
        
        // Remove enemy units
        m_units.erase(std::remove_if(m_units.begin(), m_units.end(),
            [nodeId](const Unit& u) { return u.getNodeId() == nodeId && u.getOwner() == Owner::Enemy; }),
            m_units.end());
        
        // Player units take casualties
        for (auto* unit : playerUnits) {
            int casualties = std::max(1, unit->getCount() / 3);
            unit->setCount(std::max(1, unit->getCount() - casualties));
            std::cout << "Lost " << casualties << " " << getUnitName(unit->getType()) << std::endl;
        }
        
        // Capture the node
        MapNode* node = m_map.getNodeById(nodeId);
        if (node && node->getOwner() == Owner::Enemy) {
            node->setOwner(Owner::Player);
            std::cout << "Captured " << node->getName() << "!" << std::endl;
        }
    } else {
        std::cout << ">>> DEFEAT! <<<" << std::endl;
        
        // Remove player units
        m_units.erase(std::remove_if(m_units.begin(), m_units.end(),
            [nodeId](const Unit& u) { return u.getNodeId() == nodeId && u.getOwner() == Owner::Player; }),
            m_units.end());
        
        // Enemy units take casualties
        for (auto* unit : enemyUnits) {
            int casualties = std::max(1, unit->getCount() / 4);
            unit->setCount(std::max(1, unit->getCount() - casualties));
        }
    }
}