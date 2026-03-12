#include "Game.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>

Game::Game() 
    : m_window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "City Builder")
    , m_isRunning(true)
{

    m_window.setFramerateLimit(static_cast<unsigned int>(FPS));
    

    m_map.initialize();
    
    std::cout << "\n=== City Builder Game ===" << std::endl;
    std::cout << "Starting game..." << std::endl;
    std::cout << "\n=== Node Positions ===" << std::endl;
    for (const auto& node : m_map.getNodes()) {
        std::cout << "Node " << node.getId() << " (" << node.getName() << "): " 
                  << node.getPosition().x << ", " << node.getPosition().y 
                  << " Owner: " << static_cast<int>(node.getOwner()) << std::endl;
    }
    std::cout << "=====================\n" << std::endl;
     
    m_turnNumber = 1;
    m_selectedNode = nullptr;
    m_selectedUnit = nullptr;
    m_nextUnitId = 1;
    m_mouseClickedThisFrame = false;
    m_gameOver = false;
    m_gameResult = "";
    
    m_uiRenderer.loadFont("C:/Windows/Fonts/arial.ttf");
    
    m_units.push_back(Unit(m_nextUnitId++, UnitType::Infantry, 5, Owner::Enemy, 7));
    m_units.push_back(Unit(m_nextUnitId++, UnitType::Rangers, 3, Owner::Enemy, 8));
    
    srand(static_cast<unsigned int>(time(nullptr)));
    
    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "Window size: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;

}
Game::~Game() {
    std::cout << "Game shutting down..." << std::endl;
}



void Game::run() {
    while (m_isRunning && m_window.isOpen()) {
        
        float deltaTime = m_clock.restart().asSeconds();
        
      
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
               
                sf::Vector2f mousePos(
                    static_cast<float>(mouseButtonPressed->position.x),
                    static_cast<float>(mouseButtonPressed->position.y)
                );
                
                
                bool inMenuArea = (mousePos.x >= 970 && mousePos.y >= 140 && mousePos.y <= 610);
                
                if (!inMenuArea) {
                    MapNode* clickedNode = m_map.getNodeAtPosition(mousePos);
                    
                    if (clickedNode) {
                        
                        m_selectedNode = clickedNode;
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
                                  << " (Owner: " 
                                  << (clickedNode->getOwner() == Owner::Player ? "Player" : 
                                      clickedNode->getOwner() == Owner::Enemy ? "Enemy" : "Neutral") 
                                  << ")" << std::endl;
                    } else {
                        m_selectedNode = nullptr;
                        m_selectedUnit = nullptr;
                    }
                }
            }
        }
    }
}

void Game::update(float deltaTime) {
    m_mousePosition = sf::Mouse::getPosition(m_window);
}

void Game::render() {
    m_window.clear(sf::Color(30, 40, 80));
    
        m_map.render(m_window);
   
    m_map.renderUnits(m_window, m_units);

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
    
    bool menuShown = false;
    
   
    if (!menuShown && m_selectedUnit && !m_selectedUnit->isMoving() && m_resources.actionPoints >= 1) {
        MapNode* currentNode = m_map.getNodeById(m_selectedUnit->getNodeId());
        if (currentNode) {
            // Build list of connected nodes (movement destinations)
            std::vector<int> destinations;
            std::vector<std::string> destinationNames;
            
            for (int connId : currentNode->getConnections()) {
                MapNode* destNode = m_map.getNodeById(connId);
                if (destNode) {
                    destinations.push_back(connId);
                    destinationNames.push_back(destNode->getName());
                }
            }
            
            // Render movement menu and check if user clicked a destination
            int selectedDest = m_uiRenderer.renderMoveMenu(
                m_window, currentNode, destinations, destinationNames, 
                m_mousePosition, m_mouseClickedThisFrame
            );
            
            if (selectedDest != -1) {
                moveUnit(m_selectedUnit, selectedDest);
            }
            
            menuShown = true;
        }
    }
    

    if (!menuShown && m_selectedNode && m_selectedNode->getOwner() == Owner::Player) {
        
        // If node has a Barracks -> Show recruitment menu
        if (m_selectedNode->getBuilding() == BuildingType::Barracks) {
            UnitType selectedUnitType = m_uiRenderer.renderRecruitMenu(
                m_window, m_selectedNode, m_resources, 
                m_mousePosition, m_mouseClickedThisFrame
            );
            
            if (selectedUnitType != static_cast<UnitType>(-1)) {
                recruitUnit(m_selectedNode, selectedUnitType);
            }
            menuShown = true;
        }
        // If node is empty -> Show building menu
        else if (m_selectedNode->getBuilding() == BuildingType::None) {
            BuildingType selectedBuilding = m_uiRenderer.renderBuildMenu(
                m_window, m_selectedNode, m_resources, 
                m_mousePosition, m_mouseClickedThisFrame
            );
            
            if (selectedBuilding != BuildingType::None) {
                buildBuilding(m_selectedNode, selectedBuilding);
            }
            menuShown = true;
        }
    }
    
  
    if (!menuShown && m_selectedNode && m_selectedNode->getOwner() == Owner::Neutral) {
        if (isAdjacentToPlayer(m_selectedNode)) {
            if (m_uiRenderer.renderCaptureButton(
                m_window, m_selectedNode, m_resources, 
                m_mousePosition, m_mouseClickedThisFrame)) {
                captureTerritory(m_selectedNode);
            }
            menuShown = true;
        }
    }
    
    // Render "End Turn" button (always visible)
    if (m_uiRenderer.renderEndTurnButton(m_window, m_mousePosition, m_mouseClickedThisFrame)) {
        endTurn();
    }
    
    // Game over overlay (darkens screen when game ends)
    if (m_gameOver) {
        sf::RectangleShape overlay(sf::Vector2f(1280, 720));
        overlay.setPosition(sf::Vector2f(0, 0));
        overlay.setFillColor(sf::Color(0, 0, 0, 180)); // Semi-transparent black
        m_window.draw(overlay);
        
       
    }
    
    // Reset click flag for next frame
    m_mouseClickedThisFrame = false;
    
    // Display everything to screen
    m_window.display();
}


void Game::endTurn() {
    // Don't allow ending turn if game is over
    if (m_gameOver) {
        std::cout << "Game Over! " << m_gameResult << std::endl;
        return;
    }
    
    std::cout << "\n=== End Turn " << m_turnNumber << " ===" << std::endl;
    
    // ===== 1. Resource Production =====
    int goldIncome = 0;
    int foodIncome = 0;
    int woodIncome = 0;
    
    // Sum production from all player-owned nodes
    for (const auto& node : m_map.getNodes()) {
        if (node.getOwner() == Owner::Player) {
            node.getProduction(goldIncome, foodIncome, woodIncome);
        }
    }
    
    std::cout << "Production: +" << goldIncome << " Gold, +" 
              << foodIncome << " Food, +" << woodIncome << " Wood" << std::endl;
    
    // ===== 2. Upkeep Costs =====
    // (Currently no upkeep - could add unit maintenance costs here)
    int upkeep = 0;
    std::cout << "Upkeep: -" << upkeep << " Food" << std::endl;
    
    // ===== 3. Update Resources =====
    m_resources.gold += goldIncome;
    m_resources.food += foodIncome - upkeep;
    m_resources.wood += woodIncome;
    
  
    if (m_resources.food < 0) m_resources.food = 0;
    

    m_resources.actionPoints = 3;
    

    for (auto& unit : m_units) {
        unit.updateMovement();
    }
    
    // ===== 5. Resolve Combat =====
    // Check each node for battles (player units vs enemy units)
    std::set<int> processedNodes; // Track which nodes we've checked
    for (const auto& unit : m_units) {
        // Only check nodes with arrived units (not in transit)
        if (!unit.isMoving() && processedNodes.find(unit.getNodeId()) == processedNodes.end()) {
            processCombat(unit.getNodeId());
            processedNodes.insert(unit.getNodeId());
        }
    }
    
    // ===== 6. Enemy AI Turn =====
    std::cout << "\n--- Enemy Turn ---" << std::endl;
    processEnemyTurn();
    std::cout << "--- Enemy Turn Complete ---\n" << std::endl;
    
    // ===== 7. Increment Turn Counter =====
    m_turnNumber++;
    
    std::cout << "New resources: " << m_resources.gold << " Gold, " 
              << m_resources.food << " Food, " << m_resources.wood << " Wood" << std::endl;
    std::cout << "=== Turn " << m_turnNumber << " Started ===" << std::endl;
    
    // ===== 8. Check Win/Loss Conditions =====
    checkVictoryConditions();
}


void Game::checkVictoryConditions() {
    if (m_gameOver) return; // Already ended
    
    bool playerHasNodes = false;
    bool enemyHasNodes = false;
    
    for (const auto& node : m_map.getNodes()) {
        if (node.getOwner() == Owner::Player) {
            playerHasNodes = true;
        } else if (node.getOwner() == Owner::Enemy) {
            enemyHasNodes = true;
        }
    }
    
    if (!enemyHasNodes) {
        m_gameOver = true;
        m_gameResult = "VICTORY";
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << ">>> VICTORY! <<<" << std::endl;
        std::cout << "You have conquered all enemy territories!" << std::endl;
        std::cout << "Final Turn: " << m_turnNumber << std::endl;
        std::cout << std::string(50, '=') << "\n" << std::endl;
    }
    else if (!playerHasNodes) {
        m_gameOver = true;
        m_gameResult = "DEFEAT";
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << ">>> DEFEAT! <<<" << std::endl;
        std::cout << "You have lost all your territories!" << std::endl;
        std::cout << "Survived until Turn: " << m_turnNumber << std::endl;
        std::cout << std::string(50, '=') << "\n" << std::endl;
    }
}

void Game::buildBuilding(MapNode* node, BuildingType building) {
    // Validation checks
    if (!node || node->getOwner() != Owner::Player || node->getBuilding() != BuildingType::None) {
        std::cout << "Cannot build here!" << std::endl;
        return;
    }
    
    BuildingInfo info = getBuildingInfo(building);
    
    // Check if player can afford it
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
    
    // Construct building
    node->setBuilding(building);
    
    std::cout << "Built " << info.name << " at " << node->getName() << "!" << std::endl;
    std::cout << "Remaining: " << m_resources.gold << " Gold, " 
              << m_resources.wood << " Wood, " 
              << m_resources.actionPoints << " AP" << std::endl;
}

bool Game::isAdjacentToPlayer(MapNode* node) {
    if (!node) return false;
    
    // Check all connected nodes
    for (int connectedId : node->getConnections()) {
        MapNode* connectedNode = m_map.getNodeById(connectedId);
        if (connectedNode && connectedNode->getOwner() == Owner::Player) {
            return true; // Found player-owned neighbor
        }
    }
    return false; // No player neighbors
}

void Game::captureTerritory(MapNode* node) {
    // Only neutral nodes can be peacefully captured
    if (!node || node->getOwner() != Owner::Neutral) {
        std::cout << "Can only peacefully capture neutral territory!" << std::endl;
        return;
    }
    
    // Must be adjacent to expand
    if (!isAdjacentToPlayer(node)) {
        std::cout << "Territory must be adjacent to your land!" << std::endl;
        return;
    }
    
    // Fixed cost for neutral territory
    int cost = 50;
    
    // Check resources
    if (m_resources.gold < cost) {
        std::cout << "Not enough gold! Need " << cost << "." << std::endl;
        return;
    }
    
    if (m_resources.actionPoints < 1) {
        std::cout << "Not enough action points!" << std::endl;
        return;
    }
    
    // Spend resources
    m_resources.gold -= cost;
    m_resources.actionPoints -= 1;
    
    // Capture the territory
    node->setOwner(Owner::Player);
    
    std::cout << "Captured " << node->getName() << "!" << std::endl;
    std::cout << "Cost: " << cost << " Gold" << std::endl;
    std::cout << "Remaining: " << m_resources.gold << " Gold, " 
              << m_resources.actionPoints << " AP" << std::endl;
}




void Game::recruitUnit(MapNode* node, UnitType type) {
    // Validation
    if (!node || node->getOwner() != Owner::Player || node->getBuilding() != BuildingType::Barracks) {
        std::cout << "Need a Barracks to recruit units!" << std::endl;
        return;
    }
    
    UnitStats stats = getUnitStats(type);
    
    // Check resources
    if (!m_resources.canAfford(stats.goldCost, stats.foodCost, 0)) {
        std::cout << "Not enough resources!" << std::endl;
        return;
    }
    
    // Check action points
    if (m_resources.actionPoints < 1) {
        std::cout << "Not enough action points!" << std::endl;
        return;
    }
    
    // Spend resources
    m_resources.gold -= stats.goldCost;
    m_resources.food -= stats.foodCost;
    m_resources.actionPoints -= 1;
    
    // Check if we already have units of this type at this node
    Unit* existingUnit = nullptr;
    for (auto& unit : m_units) {
        if (unit.getNodeId() == node->getId() && 
            unit.getType() == type && 
            unit.getOwner() == Owner::Player) {
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
    // Validation
    if (!unit || m_resources.actionPoints < 1) {
        std::cout << "Cannot move unit!" << std::endl;
        return;
    }
    
    MapNode* destNode = m_map.getNodeById(destinationNodeId);
    if (!destNode) return;
    
    // Spend action point
    m_resources.actionPoints -= 1;
    
    // Start movement (arrives in 1 turn)
    unit->startMoving(destinationNodeId, 1);
    
    std::cout << "Unit moving to " << destNode->getName() << "..." << std::endl;
    
    // Deselect unit (it's now in transit)
    m_selectedUnit = nullptr;
}


void Game::processCombat(int nodeId) {
    MapNode* node = m_map.getNodeById(nodeId);
    if (!node) return;
    
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

    // Player units on undefended enemy node -> instant capture
    if (!playerUnits.empty() && enemyUnits.empty() && node->getOwner() == Owner::Enemy) {
        node->setOwner(Owner::Player);
        std::cout << "\nCaptured " << node->getName() << " (undefended)!" << std::endl;
        return;
    }
    
    // Enemy units on undefended player node -> instant capture
    if (!enemyUnits.empty() && playerUnits.empty() && node->getOwner() == Owner::Player) {
        node->setOwner(Owner::Enemy);
        std::cout << "\nLost " << node->getName() << " to enemy forces!" << std::endl;
        return;
    }
    
    // No battle if only one side present
    if (playerUnits.empty() || enemyUnits.empty()) {
        return;
    }
    
    
    std::cout << "\n*** BATTLE at " << node->getName() << " ***" << std::endl;
    
    // Calculate player total power
    int playerPower = 0;
    for (auto* unit : playerUnits) {
        UnitStats stats = getUnitStats(unit->getType());
        int power = unit->getCount() * stats.attack;
        
        // Check for counter bonuses against enemy units
        for (auto* enemy : enemyUnits) {
            if (stats.counters == enemy->getType()) {
                power = static_cast<int>(power * 1.3); 
                std::cout << getUnitName(unit->getType()) << " gets counter bonus vs " 
                          << getUnitName(enemy->getType()) << "!" << std::endl;
            }
        }
        playerPower += power;
    }
    
    // Calculate enemy total power
    int enemyPower = 0;
    for (auto* unit : enemyUnits) {
        UnitStats stats = getUnitStats(unit->getType());
        int power = unit->getCount() * stats.defense;
        
        // Check for counter bonuses against player units
        for (auto* player : playerUnits) {
            if (stats.counters == player->getType()) {
                power = static_cast<int>(power * 1.5); // 50% bonus (defender advantage)
                std::cout << getUnitName(unit->getType()) << " gets counter bonus vs " 
                          << getUnitName(player->getType()) << "!" << std::endl;
            }
        }
        enemyPower += power;
    }
    
    std::cout << "Player Power: " << playerPower << " vs Enemy Power: " << enemyPower << std::endl;
    
    // ===== Resolve Battle =====
    
    if (playerPower > enemyPower) {
        // PLAYER VICTORY
        std::cout << ">>> VICTORY! <<<" << std::endl;
        
        // Remove all enemy units at this node
        m_units.erase(std::remove_if(m_units.begin(), m_units.end(),
            [nodeId](const Unit& u) { 
                return u.getNodeId() == nodeId && u.getOwner() == Owner::Enemy; 
            }),
            m_units.end());
        
        // Player units take casualties (lose 1/3 of units)
        for (auto* unit : playerUnits) {
            int casualties = std::max(1, unit->getCount() / 3);
            unit->setCount(std::max(1, unit->getCount() - casualties));
            std::cout << "Lost " << casualties << " " << getUnitName(unit->getType()) << std::endl;
        }
        
        // Capture enemy node
        if (node->getOwner() == Owner::Enemy) {
            node->setOwner(Owner::Player);
            std::cout << "Captured " << node->getName() << "!" << std::endl;
        }
    } 
    else {
        
        std::cout << ">>> DEFEAT! <<<" << std::endl;
        
        
        m_units.erase(std::remove_if(m_units.begin(), m_units.end(),
            [nodeId](const Unit& u) { 
                return u.getNodeId() == nodeId && u.getOwner() == Owner::Player; 
            }),
            m_units.end());
        
        
        for (auto* unit : enemyUnits) {
            int casualties = std::max(1, unit->getCount() / 4);
            unit->setCount(std::max(1, unit->getCount() - casualties));
        }
        
        
        if (node->getOwner() == Owner::Player) {
            node->setOwner(Owner::Enemy);
            std::cout << "Lost " << node->getName() << " to the enemy!" << std::endl;
        }
    }
}

void Game::processEnemyTurn() {
    enemyCaptureNeutral();
    enemyRecruitUnits();
    enemyMoveUnits();
}

void Game::enemyCaptureNeutral() {
  
    for (size_t i = 0; i < m_map.getNodes().size(); i++) {
        MapNode* node = m_map.getNodeById(m_map.getNodes()[i].getId());
        if (!node || node->getOwner() != Owner::Neutral) continue;
        

        bool adjacentToEnemy = false;
        for (int connId : node->getConnections()) {
            MapNode* connNode = m_map.getNodeById(connId);
            if (connNode && connNode->getOwner() == Owner::Enemy) {
                adjacentToEnemy = true;
                break;
            }
        }
        
        if (adjacentToEnemy) {
            if (rand() % 100 < 50) {
                node->setOwner(Owner::Enemy);
                std::cout << "Enemy captured " << node->getName() << "!" << std::endl;
                break; // Only capture one per turn
            }
        }
    }
}

void Game::enemyRecruitUnits() {

    for (size_t i = 0; i < m_map.getNodes().size(); i++) {
        const MapNode& node = m_map.getNodes()[i];
        if (node.getOwner() == Owner::Enemy && node.getBuilding() == BuildingType::Barracks) {
            
           
            if (rand() % 100 < 70) {
                
                UnitType types[] = {UnitType::Infantry, UnitType::Rangers, UnitType::Cavalry};
                UnitType selectedType = types[rand() % 3];
                
                
                Unit* existingUnit = nullptr;
                for (auto& unit : m_units) {
                    if (unit.getNodeId() == node.getId() && 
                        unit.getType() == selectedType && 
                        unit.getOwner() == Owner::Enemy) {
                        existingUnit = &unit;
                        break;
                    }
                }
                
                // Add to stack or create new
                if (existingUnit) {
                    existingUnit->setCount(existingUnit->getCount() + 2);
                } else {
                    m_units.push_back(Unit(m_nextUnitId++, selectedType, 2, Owner::Enemy, node.getId()));
                }
                
                std::cout << "Enemy recruited 2 " << getUnitName(selectedType) 
                          << " at " << node.getName() << std::endl;
                break; // Only recruit once per turn
            }
        }
    }
}

void Game::enemyMoveUnits() {
    // Process all enemy units
    for (auto& unit : m_units) {
        if (unit.getOwner() == Owner::Enemy && !unit.isMoving()) {
            MapNode* currentNode = m_map.getNodeById(unit.getNodeId());
            if (!currentNode) continue;
            
            // Look for best move
            int bestDestination = -1;
            
            for (int connId : currentNode->getConnections()) {
                MapNode* destNode = m_map.getNodeById(connId);
                if (!destNode) continue;
                
                // Priority 1: Attack player nodes
                if (destNode->getOwner() == Owner::Player) {
                    bestDestination = connId;
                    break; // Immediately attack if possible
                }
                // Priority 2: Expand to neutral nodes
                else if (destNode->getOwner() == Owner::Neutral && bestDestination == -1) {
                    bestDestination = connId;
                }
            }
            
            // Execute move if found a destination
            if (bestDestination != -1) {
                unit.startMoving(bestDestination, 1);
                MapNode* destNode = m_map.getNodeById(bestDestination);
                std::cout << "Enemy unit moving from " << currentNode->getName() 
                          << " to " << destNode->getName() << std::endl;
            }
        }
    }
}