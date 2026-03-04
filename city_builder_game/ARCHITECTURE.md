# City Builder Game - Project Architecture

## Overview
This document explains the architecture and design decisions for the city-building strategy game.

## Design Philosophy

### Why Node-Based Map?
Instead of a traditional grid-based system (like SimCity), we use a **node-based map** where:
- **Nodes** represent locations (cities, camps, resources)
- **Edges** represent connections between locations
- Units travel along these connections

**Benefits:**
- More strategic gameplay (limited paths)
- Easier pathfinding (just follow connections)
- Resembles classic strategy games like Risk or Advance Wars
- Better for turn-based combat

### Turn-Based Architecture
The game follows a strict turn structure:

```
Turn N:
├── Action Phase (Player)
│   ├── Spend Action Points (max 3)
│   ├── Build buildings
│   ├── Recruit units
│   └── Move units
├── End Turn (Processing)
│   ├── Move units (reduce turnsLeft)
│   ├── Generate resources from buildings
│   ├── Deduct upkeep costs
│   └── Check for battles
└── Combat Resolution
    └── Auto-resolve any battles
```

## Core Classes

### 1. Game (Game.h / Game.cpp)
**Responsibility**: Main game loop and high-level coordination

```cpp
class Game {
    void run();           // Main loop
    void processEvents(); // Handle input
    void update();        // Update game state
    void render();        // Draw everything
};
```

**Key Concepts:**
- Uses SFML's event system for input
- Delta time for frame-independent updates
- Fixed frame rate (60 FPS)

### 2. GameResources (GameResources.h)
**Responsibility**: Resource tracking and building definitions

```cpp
struct GameResources {
    int gold, food, wood, actionPoints;
    bool canAfford(int gold, int food, int wood);
    bool spend(int gold, int food, int wood);
};

enum class BuildingType {
    Castle, Farm, Mine, Lumbermill, Barracks
};
```

**Design Note**: Using structs for data, functions for behavior
- Plain Old Data (POD) structures are cache-friendly
- Easy to serialize for save/load
- Simple to pass around

### 3. MapNode (MapNode.h)
**Responsibility**: Represents a single location on the map

```cpp
class MapNode {
    int m_id;
    std::string m_name;
    sf::Vector2f m_position;  // Screen coordinates
    Owner m_owner;            // Player, Enemy, Neutral
    BuildingType m_building;
    std::vector<int> m_connections;  // Connected node IDs
};
```

**Key Features:**
- Each node knows its neighbors (connections)
- Position is in screen space (pixels) for rendering
- Owner determines who controls the node
- Building determines production

## Data Flow

### Building Construction
```
User clicks node
    ↓
Game checks if can build (owner = player, no building, has resources)
    ↓
Spend resources
    ↓
Set node.building = BuildingType::Farm
    ↓
Next turn: Generate resources from this building
```

### Unit Movement
```
User selects unit → clicks destination node
    ↓
Check if connected (node.isConnectedTo(destination))
    ↓
Set unit.destination = targetNode
Set unit.turnsLeft = 1 (or more for longer paths)
    ↓
Each turn: Reduce turnsLeft
When turnsLeft == 0: Move unit to destination
```

### Combat Resolution
```
Unit arrives at enemy node
    ↓
Find all units at that node (different owners)
    ↓
Calculate attack/defense power
Apply counter bonuses (rock-paper-scissors)
    ↓
Winner takes the node
Loser's units are destroyed/reduced
```

## Future Class Structure

### Phase 2: Add These Classes

**GameMap** (Map.h / Map.cpp)
```cpp
class GameMap {
    std::vector<MapNode> m_nodes;
    void render(sf::RenderWindow& window);
    MapNode* getNodeAt(sf::Vector2f position);
    MapNode* getNodeById(int id);
};
```

**Unit** (Unit.h / Unit.cpp)
```cpp
class Unit {
    UnitType m_type;      // Infantry, Rangers, Cavalry
    int m_count;          // Stack size
    int m_nodeId;         // Current location
    int m_destinationId;  // Where moving to
    int m_turnsLeft;      // Turns until arrival
};
```

**GameState** (GameState.h / GameState.cpp)
```cpp
class GameState {
    GameResources m_resources;
    GameMap m_map;
    std::vector<Unit> m_units;
    int m_turnNumber;
    
    void endTurn();
    void processResources();
    void processCombat();
};
```

**UI/Renderer** (UIRenderer.h / UIRenderer.cpp)
```cpp
class UIRenderer {
    void renderResources(const GameResources& res);
    void renderNodes(const GameMap& map);
    void renderUnits(const std::vector<Unit>& units);
    void renderButton(const std::string& text, sf::Vector2f pos);
};
```

## File Organization Best Practices

```
include/
├── Core/
│   ├── Game.h
│   └── GameState.h
├── Map/
│   ├── MapNode.h
│   ├── Map.h
│   └── PathFinder.h
├── Units/
│   ├── Unit.h
│   └── UnitTypes.h
├── Resources/
│   ├── GameResources.h
│   └── BuildingTypes.h
└── UI/
    ├── UIRenderer.h
    └── Button.h

src/
├── Core/
├── Map/
├── Units/
├── Resources/
└── UI/
```

## Memory Management Strategy

### Use Smart Pointers
```cpp
// For dynamic objects
std::unique_ptr<GameState> m_gameState;

// For shared ownership (rare)
std::shared_ptr<Texture> m_sharedTexture;
```

### Use Vectors for Collections
```cpp
// Better than manual arrays
std::vector<MapNode> m_nodes;
std::vector<Unit> m_units;
```

### Avoid Raw Pointers
- Use references when you don't own the object
- Use smart pointers when you do own it

## Rendering Strategy

### Layer System
```
Layer 1: Background (map terrain)
Layer 2: Connections (lines between nodes)
Layer 3: Nodes (circles)
Layer 4: Buildings (icons on nodes)
Layer 5: Units (icons near nodes)
Layer 6: UI (resources, buttons)
Layer 7: Tooltips (on hover)
```

### SFML Drawing Order
```cpp
void render() {
    window.clear();
    
    // Draw from back to front
    drawBackground();
    drawConnections();
    drawNodes();
    drawBuildings();
    drawUnits();
    drawUI();
    
    window.display();
}
```

## Performance Considerations

### What to Optimize Later
1. **Spatial Partitioning**: Only render visible nodes
2. **Batch Rendering**: Draw all nodes in one call
3. **Texture Atlas**: Load all sprites in one texture
4. **Object Pooling**: Reuse Unit objects instead of creating/destroying

### What NOT to Worry About Now
- Don't optimize prematurely
- Focus on clear, readable code first
- Profile before optimizing

## Next Implementation Steps

### Step 1: Rendering (Next Priority)
1. Create GameMap class
2. Initialize 7 nodes with positions
3. Render nodes as circles (sf::CircleShape)
4. Render connections as lines (sf::VertexArray)
5. Color nodes by owner (blue/red/gray)

### Step 2: Basic Interaction
1. Detect mouse position
2. Find which node is under cursor
3. Highlight selected node
4. Show node info in UI panel

### Step 3: Building System
1. Add "Build" buttons in UI
2. Check requirements (owner, resources, action points)
3. Place building on node
4. Update visual (icon on node)

### Step 4: Turn System
1. Add "End Turn" button
2. Increment turn counter
3. Calculate resource production
4. Reset action points

Would you like me to implement any of these next steps?
