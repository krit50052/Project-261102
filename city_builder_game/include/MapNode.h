#pragma once

#include "GameResources.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Owner of a node
enum class Owner {
    Player,
    Enemy,
    Neutral
};

// Map node - represents a location on the map
class MapNode {
public:
    MapNode(int id, const std::string& name, sf::Vector2f position, Owner owner = Owner::Neutral)
        : m_id(id)
        , m_name(name)
        , m_position(position)
        , m_owner(owner)
        , m_building(BuildingType::None)
    {}
    
    // Getters
    int getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    sf::Vector2f getPosition() const { return m_position; }
    Owner getOwner() const { return m_owner; }
    BuildingType getBuilding() const { return m_building; }
    const std::vector<int>& getConnections() const { return m_connections; }
    
    // Setters
    void setOwner(Owner owner) { m_owner = owner; }
    void setBuilding(BuildingType building) { m_building = building; }
    
    // Connections
    void addConnection(int nodeId) {
        // Avoid duplicates
        for (int id : m_connections) {
            if (id == nodeId) return;
        }
        m_connections.push_back(nodeId);
    }
    
    bool isConnectedTo(int nodeId) const {
        for (int id : m_connections) {
            if (id == nodeId) return true;
        }
        return false;
    }
    
    // Check if this node can build
    bool canBuild() const {
        return m_owner == Owner::Player && m_building == BuildingType::None;
    }
    
    // Get production from this node
    void getProduction(int& gold, int& food, int& wood) const {
        if (m_owner == Owner::Player && m_building != BuildingType::None) {
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
