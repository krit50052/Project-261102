#pragma once

#include <string>

enum class BuildingType {
    None,       
    Castle,      
    Farm,       
    Mine,        
    Lumbermill,  
    Barracks    
};

struct BuildingInfo {
    std::string name;        
    int goldCost;        
    int woodCost;            
    int goldProduction;      
    int foodProduction;      
    int woodProduction;    
};

inline BuildingInfo getBuildingInfo(BuildingType type) {
    switch (type) {
        case BuildingType::Castle:
            return {"Castle", 0, 0, 5, 5, 0};
        case BuildingType::Farm:
            return {"Farm", 30, 10, 0, 10, 0};
        case BuildingType::Mine:
            return {"Mine", 40, 15, 15, 0, 0};
        case BuildingType::Lumbermill:
            return {"Lumbermill", 35, 5, 0, 0, 8};
        case BuildingType::Barracks:
            return {"Barracks", 50, 20, 0, 0, 0};
        default:
            return {"None", 0, 0, 0, 0, 0};
    }
}

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
    
    bool canAfford(int goldCost, int foodCost, int woodCost) const {
        return gold >= goldCost && food >= foodCost && wood >= woodCost;
    }
};