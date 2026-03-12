#include "UIRenderer.h"
#include <iostream>

UIRenderer::UIRenderer() 
    : m_fontLoaded(false)
{
}

UIRenderer::~UIRenderer() {
}

bool UIRenderer::loadFont(const std::string& fontPath) {
    if (m_font.openFromFile(fontPath)) {
        m_fontLoaded = true;
        std::cout << "Font loaded successfully!" << std::endl;
        return true;
    }
    std::cerr << "Failed to load font from: " << fontPath << std::endl;
    return false;
}

void UIRenderer::renderTopBar(sf::RenderWindow& window, const GameResources& resources, int turnNumber) {
    sf::RectangleShape topBar(sf::Vector2f(1280, 60));
    topBar.setPosition(sf::Vector2f(0, 0));
    topBar.setFillColor(sf::Color(20, 25, 35, 220));
    window.draw(topBar);
    
    if (!m_fontLoaded) {
        return;
    }
    
    sf::Text turnText(m_font);
    turnText.setString("Turn " + std::to_string(turnNumber));
    turnText.setCharacterSize(24);
    turnText.setFillColor(sf::Color::White);
    turnText.setPosition(sf::Vector2f(20, 15));
    window.draw(turnText);
    
    int startX = 300;
    drawResourceBar(window, startX, 15, "Gold", resources.gold, sf::Color(255, 215, 0));
    drawResourceBar(window, startX + 200, 15, "Food", resources.food, sf::Color(100, 200, 100));
    drawResourceBar(window, startX + 400, 15, "Wood", resources.wood, sf::Color(139, 90, 43));
    
    sf::Text apText(m_font);
    apText.setString("AP: " + std::to_string(resources.actionPoints));
    apText.setCharacterSize(24);
    apText.setFillColor(sf::Color(200, 150, 255));
    apText.setPosition(sf::Vector2f(1150, 15));
    window.draw(apText);
}

void UIRenderer::drawResourceBar(sf::RenderWindow& window, int x, int y, 
                                 const std::string& label, int value, sf::Color color) {
    sf::CircleShape icon(12);
    icon.setPosition(sf::Vector2f(x, y + 5));
    icon.setFillColor(color);
    window.draw(icon);
    
    sf::Text text(m_font);
    text.setString(label + ": " + std::to_string(value));
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(x + 30, y));
    window.draw(text);
}


void UIRenderer::renderNodeInfo(sf::RenderWindow& window, MapNode* node) {
    if (!m_fontLoaded || !node) {
        return;
    }
    
    sf::RectangleShape panel(sf::Vector2f(300, 180));
    panel.setPosition(sf::Vector2f(20, 540));
    panel.setFillColor(sf::Color(20, 25, 35, 230));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(panel);
    
    sf::Text nameText(m_font);
    nameText.setString(node->getName());
    nameText.setCharacterSize(24);
    nameText.setFillColor(sf::Color::White);
    nameText.setStyle(sf::Text::Bold);
    nameText.setPosition(sf::Vector2f(35, 555));
    window.draw(nameText);
    
    sf::Text ownerText(m_font);
    std::string ownerStr = "Owner: ";
    sf::Color ownerColor;
    
    switch (node->getOwner()) {
        case Owner::Player:
            ownerStr += "Player";
            ownerColor = sf::Color(60, 120, 216); // Blue
            break;
        case Owner::Enemy:
            ownerStr += "Enemy";
            ownerColor = sf::Color(220, 60, 60); // Red
            break;
        case Owner::Neutral:
            ownerStr += "Neutral";
            ownerColor = sf::Color(160, 160, 170); // Gray
            break;
    }
    
    ownerText.setString(ownerStr);
    ownerText.setCharacterSize(18);
    ownerText.setFillColor(ownerColor);
    ownerText.setPosition(sf::Vector2f(35, 595));
    window.draw(ownerText);
    
    sf::Text buildingText(m_font);
    std::string buildingStr = "Building: ";
    
    if (node->getBuilding() != BuildingType::None) {
        BuildingInfo info = getBuildingInfo(node->getBuilding());
        buildingStr += info.name;
        
        // Show production rates
        if (info.goldProduction > 0 || info.foodProduction > 0 || info.woodProduction > 0) {
            buildingStr += "\nProduction: ";
            if (info.goldProduction > 0) buildingStr += "+" + std::to_string(info.goldProduction) + " Gold ";
            if (info.foodProduction > 0) buildingStr += "+" + std::to_string(info.foodProduction) + " Food ";
            if (info.woodProduction > 0) buildingStr += "+" + std::to_string(info.woodProduction) + " Wood";
        }
    } else {
        buildingStr += "None";
    }
    
    buildingText.setString(buildingStr);
    buildingText.setCharacterSize(16);
    buildingText.setFillColor(sf::Color(200, 200, 200));
    buildingText.setPosition(sf::Vector2f(35, 625));
    window.draw(buildingText);
}

BuildingType UIRenderer::renderBuildMenu(sf::RenderWindow& window, MapNode* node, const GameResources& resources, sf::Vector2i mousePos, bool mouseClicked) {
    // Only show for empty player-owned nodes
    if (!m_fontLoaded || !node || node->getOwner() != Owner::Player || node->getBuilding() != BuildingType::None) {
        return BuildingType::None;
    }
    
    sf::RectangleShape panel(sf::Vector2f(280, 400));
    panel.setPosition(sf::Vector2f(980, 150));
    panel.setFillColor(sf::Color(20, 25, 35, 230));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(panel);
    
    sf::Text title(m_font);
    title.setString("Build Menu");
    title.setCharacterSize(22);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(sf::Vector2f(1020, 165));
    window.draw(title);
    
    BuildingType buildings[] = {
        BuildingType::Farm, 
        BuildingType::Mine, 
        BuildingType::Lumbermill, 
        BuildingType::Barracks
	
    };
    BuildingType selectedBuilding = BuildingType::None;
    
    for (int i = 0; i < 4; i++) {
        BuildingInfo info = getBuildingInfo(buildings[i]);
        
        float yPos = 210 + i * 90;
        sf::Vector2f buttonPos(1000, yPos);
        sf::Vector2f buttonSize(240, 75);
        
        bool isHovered = mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
                         mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
        
        bool canAfford = resources.canAfford(info.goldCost, 0, info.woodCost) && resources.actionPoints >= 1;
        
        sf::RectangleShape button(buttonSize);
        button.setPosition(buttonPos);
        
        if (!canAfford) {
            button.setFillColor(sf::Color(60, 40, 40)); // Dark red = can't afford
        } else if (isHovered) {
            button.setFillColor(sf::Color(80, 100, 140)); // Highlighted = hovering
        } else {
            button.setFillColor(sf::Color(50, 60, 80)); // Normal state
        }
        
        button.setOutlineThickness(2.0f);
        button.setOutlineColor(canAfford ? sf::Color(100, 120, 150) : sf::Color(100, 60, 60));
        window.draw(button);
        
        sf::Text nameText(m_font);
        nameText.setString(info.name);
        nameText.setCharacterSize(18);
        nameText.setFillColor(canAfford ? sf::Color::White : sf::Color(150, 100, 100));
        nameText.setStyle(sf::Text::Bold);
        nameText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 8));
        window.draw(nameText);
        
        sf::Text costText(m_font);
        std::string costStr = "Cost: " + std::to_string(info.goldCost) + " Gold";
        if (info.woodCost > 0) {
            costStr += ", " + std::to_string(info.woodCost) + " Wood";
        }
        costText.setString(costStr);
        costText.setCharacterSize(14);
        costText.setFillColor(sf::Color(200, 200, 200));
        costText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 32));
        window.draw(costText);
        
        sf::Text prodText(m_font);
        std::string prodStr = "Produces: ";
        if (info.goldProduction > 0) prodStr += "+" + std::to_string(info.goldProduction) + " Gold ";
        if (info.foodProduction > 0) prodStr += "+" + std::to_string(info.foodProduction) + " Food ";
        if (info.woodProduction > 0) prodStr += "+" + std::to_string(info.woodProduction) + " Wood";
        prodText.setString(prodStr);
        prodText.setCharacterSize(13);
        prodText.setFillColor(sf::Color(150, 200, 150));
        prodText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 52));
        window.draw(prodText);
        
        if (isHovered && mouseClicked && canAfford) {
            selectedBuilding = buildings[i];
        }
    }
    
    return selectedBuilding;
}

UnitType UIRenderer::renderRecruitMenu(sf::RenderWindow& window, MapNode* node, const GameResources& resources, sf::Vector2i mousePos, bool mouseClicked) {
    // Only show for player barracks
    if (!m_fontLoaded || !node || node->getOwner() != Owner::Player || node->getBuilding() != BuildingType::Barracks) {
        return static_cast<UnitType>(-1);
    }
    
    // Menu panel
    sf::RectangleShape panel(sf::Vector2f(280, 380));
    panel.setPosition(sf::Vector2f(980, 150));
    panel.setFillColor(sf::Color(20, 25, 35, 230));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(panel);
    
    sf::Text title(m_font);
    title.setString("Recruit Units");
    title.setCharacterSize(22);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(sf::Vector2f(1010, 165));
    window.draw(title);
    
    UnitType units[] = {UnitType::Infantry, UnitType::Rangers, UnitType::Cavalry, UnitType::Spearmen};
    UnitType selectedUnit = static_cast<UnitType>(-1);
    
    for (int i = 0; i < 4; i++) {
        UnitStats stats = getUnitStats(units[i]);
        std::string name = getUnitName(units[i]);
        
        float yPos = 210 + i * 85;
        sf::Vector2f buttonPos(1000, yPos);
        sf::Vector2f buttonSize(240, 70);
        
        bool isHovered = mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
                         mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
        
        bool canAfford = resources.canAfford(stats.goldCost, stats.foodCost, 0) && resources.actionPoints >= 1;
        
        // Button
        sf::RectangleShape button(buttonSize);
        button.setPosition(buttonPos);
        
        if (!canAfford) {
            button.setFillColor(sf::Color(60, 40, 40));
        } else if (isHovered) {
            button.setFillColor(sf::Color(80, 100, 140));
        } else {
            button.setFillColor(sf::Color(50, 60, 80));
        }
        
        button.setOutlineThickness(2.0f);
        button.setOutlineColor(canAfford ? sf::Color(100, 120, 150) : sf::Color(100, 60, 60));
        window.draw(button);
        
        // Unit name (recruits 3 units)
        sf::Text nameText(m_font);
        nameText.setString(name + " (x3)");
        nameText.setCharacterSize(16);
        nameText.setFillColor(canAfford ? sf::Color::White : sf::Color(150, 100, 100));
        nameText.setStyle(sf::Text::Bold);
        nameText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 8));
        window.draw(nameText);
        
        // Cost
        sf::Text costText(m_font);
        costText.setString("Cost: " + std::to_string(stats.goldCost) + " Gold, " + std::to_string(stats.foodCost) + " Food");
        costText.setCharacterSize(13);
        costText.setFillColor(sf::Color(200, 200, 200));
        costText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 30));
        window.draw(costText);
        
        // Combat stats and counter information
        sf::Text statsText(m_font);
        statsText.setString("ATK:" + std::to_string(stats.attack) + " DEF:" + std::to_string(stats.defense) + 
                           " Counters:" + getUnitName(stats.counters));
        statsText.setCharacterSize(11);
        statsText.setFillColor(sf::Color(150, 200, 150));
        statsText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 50));
        window.draw(statsText);
        
        if (isHovered && mouseClicked && canAfford) {
            selectedUnit = units[i];
        }
    }
    
    return selectedUnit;
}

bool UIRenderer::renderCaptureButton(sf::RenderWindow& window, MapNode* node, const GameResources& resources, sf::Vector2i mousePos, bool mouseClicked) {
    // Only show for neutral nodes
    if (!m_fontLoaded || !node || node->getOwner() != Owner::Neutral) {
        return false;
    }
    
    // Capture panel
    sf::RectangleShape panel(sf::Vector2f(280, 200));
    panel.setPosition(sf::Vector2f(980, 150));
    panel.setFillColor(sf::Color(20, 25, 35, 230));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(panel);
    
    // Title
    sf::Text title(m_font);
    title.setString("Capture Territory");
    title.setCharacterSize(22);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(sf::Vector2f(1000, 165));
    window.draw(title);
    
    // Info text
    sf::Text infoText(m_font);
    infoText.setString("Expand your empire!\n\nCost: 50 Gold, 1 AP");
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color(200, 200, 200));
    infoText.setPosition(sf::Vector2f(1000, 205));
    window.draw(infoText);
    
    // Capture button
    sf::Vector2f buttonPos(1020, 280);
    sf::Vector2f buttonSize(220, 50);
    
    bool isHovered = mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
                     mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
    
    bool canAfford = resources.gold >= 50 && resources.actionPoints >= 1;
    
    sf::RectangleShape button(buttonSize);
    button.setPosition(buttonPos);
    
    if (!canAfford) {
        button.setFillColor(sf::Color(80, 40, 40));
    } else if (isHovered) {
        button.setFillColor(sf::Color(100, 140, 220)); // Bright blue highlight
    } else {
        button.setFillColor(sf::Color(60, 100, 180)); // Blue
    }
    
    button.setOutlineThickness(3.0f);
    button.setOutlineColor(canAfford ? sf::Color(80, 120, 200) : sf::Color(100, 60, 60));
    window.draw(button);
    
    sf::Text buttonText(m_font);
    buttonText.setString("Capture!");
    buttonText.setCharacterSize(22);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setStyle(sf::Text::Bold);
    
    // Center text in button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(sf::Vector2f(
        buttonPos.x + (buttonSize.x - textBounds.size.x) / 2 - textBounds.position.x,
        buttonPos.y + (buttonSize.y - textBounds.size.y) / 2 - textBounds.position.y
    ));
    
    window.draw(buttonText);
    
    return isHovered && mouseClicked && canAfford;
}
int UIRenderer::renderMoveMenu(sf::RenderWindow& window, MapNode* currentNode, 
                               const std::vector<int>& destinations, 
                               const std::vector<std::string>& destinationNames, 
                               sf::Vector2i mousePos, bool mouseClicked) {
    if (!m_fontLoaded || !currentNode || destinations.empty()) {
        return -1;
    }
    
    // Dynamic panel height based on number of destinations
    sf::RectangleShape panel(sf::Vector2f(280, 100 + destinations.size() * 60));
    panel.setPosition(sf::Vector2f(980, 150));
    panel.setFillColor(sf::Color(20, 25, 35, 230));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(panel);
    
    // Title
    sf::Text title(m_font);
    title.setString("Move Unit");
    title.setCharacterSize(22);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(sf::Vector2f(1020, 165));
    window.draw(title);
    
    int selectedDestination = -1;
    
    // Render each movement option
    for (size_t i = 0; i < destinations.size(); i++) {
        float yPos = 210 + i * 60;
        sf::Vector2f buttonPos(1000, yPos);
        sf::Vector2f buttonSize(240, 50);
        
        bool isHovered = mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
                         mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
        
        sf::RectangleShape button(buttonSize);
        button.setPosition(buttonPos);
        
        if (isHovered) {
            button.setFillColor(sf::Color(80, 120, 100)); 
        } else {
            button.setFillColor(sf::Color(50, 80, 60));
        }
        
        button.setOutlineThickness(2.0f);
        button.setOutlineColor(sf::Color(100, 150, 120));
        window.draw(button);
        
        // Destination name
        sf::Text nameText(m_font);
        nameText.setString("Move to " + destinationNames[i]);
        nameText.setCharacterSize(16);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 8));
        window.draw(nameText);
        
        // Cost info
        sf::Text costText(m_font);
        costText.setString("Cost: 1 AP, 1 Turn");
        costText.setCharacterSize(13);
        costText.setFillColor(sf::Color(200, 200, 200));
        costText.setPosition(sf::Vector2f(buttonPos.x + 10, buttonPos.y + 28));
        window.draw(costText);
        
        if (isHovered && mouseClicked) {
            selectedDestination = destinations[i];
        }
    }
    
    return selectedDestination;
}

bool UIRenderer::renderEndTurnButton(sf::RenderWindow& window, sf::Vector2i mousePos, bool mouseClicked) {
    if (!m_fontLoaded) {
        return false;
    }
    
    // Button position (bottom right)
    sf::Vector2f buttonPos(1050, 640);
    sf::Vector2f buttonSize(200, 50);
    
    // Check hover
    bool isHovered = mousePos.x >= buttonPos.x && mousePos.x <= buttonPos.x + buttonSize.x &&
                     mousePos.y >= buttonPos.y && mousePos.y <= buttonPos.y + buttonSize.y;
    
    // Button background (green, brighter when hovered)
    sf::RectangleShape button(buttonSize);
    button.setPosition(buttonPos);
    
    if (isHovered) {
        button.setFillColor(sf::Color(80, 180, 80)); 
    } else {
        button.setFillColor(sf::Color(60, 150, 60)); 
    }
    
    button.setOutlineThickness(3.0f);
    button.setOutlineColor(sf::Color(40, 100, 40));
    window.draw(button);
    
    // Button text
    sf::Text buttonText(m_font);
    buttonText.setString("End Turn");
    buttonText.setCharacterSize(24);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setStyle(sf::Text::Bold);
    
    // Center text in button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(sf::Vector2f(
        buttonPos.x + (buttonSize.x - textBounds.size.x) / 2 - textBounds.position.x,
        buttonPos.y + (buttonSize.y - textBounds.size.y) / 2 - textBounds.position.y
    ));
    
    window.draw(buttonText);
    
    // Return true if clicked
    return isHovered && mouseClicked;
}