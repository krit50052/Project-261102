#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "GameResources.h"


enum class Owner {
    Player,   
    Enemy,    
    Neutral   
};

class MapNode {
public:
    MapNode(int id, const std::string& name, sf::Vector2f position, Owner owner)
        : m_id(id)
        , m_name(name)
        , m_position(position)
        , m_owner(owner)
        , m_building(BuildingType::None)
    {}
    int getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    sf::Vector2f getPosition() const { return m_position; }
    Owner getOwner() const { return m_owner; }
    BuildingType getBuilding() const { return m_building; }
    const std::vector<int>& getConnections() const { return m_connections; }

    void setOwner(Owner owner) { m_owner = owner; }
 
    void setBuilding(BuildingType building) { m_building = building; }


    void addConnection(int nodeId) {
        m_connections.push_back(nodeId);
    }

    void getProduction(int& gold, int& food, int& wood) const {
        if (m_building != BuildingType::None) {
            BuildingInfo info = getBuildingInfo(m_building);
            gold += info.goldProduction;
            food += info.foodProduction;
            wood += info.woodProduction;
        }
    }

private:
    int m_id;                       
    std::string m_name;             
    sf::Vector2f m_position;         
    Owner m_owner;                  
    BuildingType m_building;         
    std::vector<int> m_connections;  
};