#pragma once

#include "MapNode.h"
#include <string>

enum class UnitType {
    Infantry,
    Rangers,
    Cavalry,
    Spearmen
};

struct UnitStats {
    int attack;
    int defense;
    UnitType counters;
    int goldCost;
    int foodCost;
};

inline UnitStats getUnitStats(UnitType type) {
    switch (type) {
        case UnitType::Infantry:
            return {10, 15, UnitType::Cavalry, 20, 5};
        case UnitType::Rangers:
            return {15, 8, UnitType::Infantry, 25, 5};
        case UnitType::Cavalry:
            return {20, 10, UnitType::Rangers, 35, 8};
        case UnitType::Spearmen:
            return {12, 20, UnitType::Cavalry, 22, 6};
    }
    return {0, 0, UnitType::Infantry, 0, 0};
}

inline std::string getUnitName(UnitType type) {
    switch (type) {
        case UnitType::Infantry: return "Infantry";
        case UnitType::Rangers: return "Rangers";
        case UnitType::Cavalry: return "Cavalry";
        case UnitType::Spearmen: return "Spearmen";
    }
    return "Unknown";
}

class Unit {
public:
    Unit(int id, UnitType type, int count, Owner owner, int nodeId)
        : m_id(id)
        , m_type(type)
        , m_count(count)
        , m_owner(owner)
        , m_nodeId(nodeId)
        , m_isMoving(false)
        , m_destinationNodeId(-1)
        , m_turnsUntilArrival(0)
    {}

    // Getters
    int getId() const { return m_id; }
    UnitType getType() const { return m_type; }
    int getCount() const { return m_count; }
    Owner getOwner() const { return m_owner; }
    int getNodeId() const { return m_nodeId; }
    bool isMoving() const { return m_isMoving; }
    int getDestinationNodeId() const { return m_destinationNodeId; }
    int getTurnsUntilArrival() const { return m_turnsUntilArrival; }

    // Setters
    void setCount(int count) { m_count = count; }
    void setNodeId(int nodeId) { m_nodeId = nodeId; }
    
    // Movement
    void startMoving(int destinationNodeId, int turns) {
        m_isMoving = true;
        m_destinationNodeId = destinationNodeId;
        m_turnsUntilArrival = turns;
    }
    
    void updateMovement() {
        if (m_isMoving && m_turnsUntilArrival > 0) {
            m_turnsUntilArrival--;
            if (m_turnsUntilArrival == 0) {
                // Arrived
                m_nodeId = m_destinationNodeId;
                m_isMoving = false;
                m_destinationNodeId = -1;
            }
        }
    }

private:
    int m_id;
    UnitType m_type;
    int m_count;
    Owner m_owner;
    int m_nodeId;
    bool m_isMoving;
    int m_destinationNodeId;
    int m_turnsUntilArrival;
};