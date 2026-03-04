#pragma once

#include <string>

// Struct to hold game resources
struct GameResources {
    int gold;
    int food;
    int wood;
    int actionPoints;
    
    GameResources() 
        : gold(100)
        , food(50)
        , wood(30)
        , actionPoints(3)
    {}
    
    // Check if we can afford something
    bool canAfford(int goldCost, int foodCost = 0, int woodCost = 0) const {
        return gold >= goldCost && food >= foodCost && wood >= woodCost;
    }
    
    // Spend resources
    bool spend(int goldCost, int foodCost = 0, int woodCost = 0) {
        if (!canAfford(goldCost, foodCost, woodCost)) {
            return false;
        }
        gold -= goldCost;
        food -= foodCost;
        wood -= woodCost;
        return true;
    }
    
    // Add resources
    void add(int goldAmount, int foodAmount = 0, int woodAmount = 0) {
        gold += goldAmount;
        food += foodAmount;
        wood += woodAmount;
    }
};

// Building types
enum class BuildingType {
    None,
    Castle,
    Farm,
    Mine,
    Lumbermill,
    Barracks
};

// Building production info
struct BuildingInfo {
    BuildingType type;
    std::string name;
    int goldCost;
    int woodCost;
    int goldProduction;
    int foodProduction;
    int woodProduction;
    
    BuildingInfo(BuildingType t, const std::string& n, 
                 int gCost, int wCost,
                 int gProd = 0, int fProd = 0, int wProd = 0)
        : type(t), name(n)
        , goldCost(gCost), woodCost(wCost)
        , goldProduction(gProd), foodProduction(fProd), woodProduction(wProd)
    {}
};

// Get building info by type
inline BuildingInfo getBuildingInfo(BuildingType type) {
    switch (type) {
        case BuildingType::Castle:
            return BuildingInfo(BuildingType::Castle, "Castle", 0, 0, 5, 5, 0);
        case BuildingType::Farm:
            return BuildingInfo(BuildingType::Farm, "Farm", 30, 10, 0, 10, 0);
        case BuildingType::Mine:
            return BuildingInfo(BuildingType::Mine, "Mine", 40, 15, 15, 0, 0);
        case BuildingType::Lumbermill:
            return BuildingInfo(BuildingType::Lumbermill, "Lumbermill", 35, 5, 0, 0, 8);
        case BuildingType::Barracks:
            return BuildingInfo(BuildingType::Barracks, "Barracks", 50, 20, 0, 0, 0);
        default:
            return BuildingInfo(BuildingType::None, "Empty", 0, 0, 0, 0, 0);
    }
}
