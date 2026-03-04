# City Builder Game - Demacia Rising Style

A turn-based strategy city-building game built with C++ and SFML, inspired by resource management and auto-battler mechanics.

## Project Structure

```
city_builder_game/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── Game.h             # Main game class
│   ├── GameResources.h    # Resource management
│   └── MapNode.h          # Map node system
├── src/                    # Source files
│   ├── main.cpp           # Entry point
│   └── Game.cpp           # Game implementation
├── resources/              # Game assets
│   ├── fonts/
│   └── textures/
└── build/                  # Build output (generated)
```

## Core Game Systems

### 1. **Resource Management**
- **Gold**: Currency for building and recruiting
- **Food**: Required for unit upkeep
- **Wood**: Building material
- **Action Points**: Limit actions per turn (default: 3)

### 2. **Map System**
- Node-based map with connected locations
- Three ownership states: Player, Enemy, Neutral
- Buildings can be constructed on player-owned nodes

### 3. **Buildings**
- **Castle**: +5 Gold, +5 Food per turn
- **Farm**: +10 Food per turn (Cost: 30 Gold, 10 Wood)
- **Mine**: +15 Gold per turn (Cost: 40 Gold, 15 Wood)
- **Lumbermill**: +8 Wood per turn (Cost: 35 Gold, 5 Wood)
- **Barracks**: Recruit units (Cost: 50 Gold, 20 Wood)

### 4. **Turn System**
Each turn consists of:
1. **Action Phase**: Spend action points to build, recruit, or move units
2. **End Turn**: Process resource generation, unit movement, and combat
3. **Resolution**: Battles auto-resolve based on unit stats and counters

### 5. **Combat System** (To be implemented)
Auto-battler with rock-paper-scissors mechanics:
- **Infantry** counters Cavalry
- **Rangers** counter Infantry  
- **Cavalry** counters Rangers
- **Spearmen** counter Cavalry

## Requirements

- **C++17** or later
- **CMake 3.16** or later
- **SFML 2.5** or later

## Building the Project

### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake libsfml-dev

# Build
cd city_builder_game
mkdir -p build
cd build
cmake ..
make

# Run
./CityBuilderGame
```

### macOS (with Homebrew)
```bash
# Install dependencies
brew install cmake sfml

# Build
cd city_builder_game
mkdir -p build
cd build
cmake ..
make

# Run
./CityBuilderGame
```

### Windows (with MSYS2/MinGW)
```bash
# Install dependencies (in MSYS2 terminal)
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-sfml

# Build
cd city_builder_game
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make

# Run
./CityBuilderGame.exe
```

## Development Roadmap

### Phase 1: Foundation (Current)
- [x] Basic SFML window and game loop
- [x] Resource management system
- [x] Map node data structures
- [ ] Basic rendering of nodes and connections
- [ ] UI for resources display

### Phase 2: Core Gameplay
- [ ] Building placement system
- [ ] Turn management
- [ ] Resource generation per turn
- [ ] Mouse interaction (node selection)
- [ ] Action point system

### Phase 3: Units & Combat
- [ ] Unit data structures
- [ ] Unit recruitment at barracks
- [ ] Unit movement system
- [ ] Auto-battler combat resolution
- [ ] Combat animations

### Phase 4: Polish
- [ ] Sound effects and music
- [ ] Better graphics/sprites
- [ ] Save/load system
- [ ] AI for enemy turns
- [ ] Victory/defeat conditions

## Controls

- **ESC**: Quit game
- **Mouse**: Select nodes (to be implemented)
- **Click**: Build/recruit/move (to be implemented)

## Next Steps

To continue development:
1. Implement node rendering with SFML shapes
2. Add UI for displaying resources
3. Implement mouse interaction for selecting nodes
4. Add building construction mechanics
5. Implement turn system with "End Turn" button

## License

Educational project - feel free to use and modify!
