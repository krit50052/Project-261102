# Setup Guide - City Builder Game

## Prerequisites Installation

### Ubuntu/Debian Linux
```bash
# Update package list
sudo apt-get update

# Install build essentials
sudo apt-get install -y build-essential cmake

# Install SFML development libraries
sudo apt-get install -y libsfml-dev

# Verify installation
pkg-config --modversion sfml-all
```

### macOS
```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install CMake and SFML
brew install cmake sfml

# Verify installation
brew list sfml
```

### Windows
**Option 1: MSYS2 (Recommended)**
```bash
# Download and install MSYS2 from https://www.msys2.org/

# Open MSYS2 MinGW 64-bit terminal
# Update package database
pacman -Syu

# Install build tools and SFML
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-sfml

# Add to PATH: C:\msys64\mingw64\bin
```

**Option 2: Visual Studio**
- Download SFML from https://www.sfml-dev.org/download.php
- Extract to C:\SFML
- Configure CMake to find SFML manually

## Building the Project

### Step 1: Navigate to Project Directory
```bash
cd /path/to/city_builder_game
```

### Step 2: Build (Linux/macOS)
```bash
# Make the build script executable (first time only)
chmod +x build.sh

# Run the build script
./build.sh
```

Or manually:
```bash
mkdir -p build
cd build
cmake ..
make
```

### Step 3: Build (Windows - MSYS2)
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

### Step 4: Run the Game
```bash
# Linux/macOS
./build/CityBuilderGame

# Windows
./build/CityBuilderGame.exe
```

## Troubleshooting

### Problem: "SFML not found" during CMake
**Solution:**
```bash
# Linux: Make sure SFML is installed
sudo apt-get install libsfml-dev

# macOS: 
brew install sfml

# Windows: Make sure SFML is in your PATH or tell CMake where it is:
cmake .. -DSFML_DIR="C:/SFML/lib/cmake/SFML"
```

### Problem: "Command not found: cmake"
**Solution:**
```bash
# Linux
sudo apt-get install cmake

# macOS
brew install cmake

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-cmake
```

### Problem: Black screen or window doesn't open
**Solution:**
- Check that SFML graphics libraries are properly linked
- Make sure your graphics drivers are up to date
- Try running with: `./CityBuilderGame --verbose`

### Problem: Build errors about C++17
**Solution:**
Make sure you have a modern compiler:
```bash
# Check version
g++ --version  # Should be 7.0 or higher

# Linux: Update if needed
sudo apt-get install g++-9

# macOS: Should be fine with Xcode command line tools
xcode-select --install
```

## Project Structure After Build

```
city_builder_game/
├── build/
│   ├── CityBuilderGame          # The executable (Linux/macOS)
│   ├── CityBuilderGame.exe      # The executable (Windows)
│   ├── CMakeFiles/              # CMake temporary files
│   ├── resources/               # Copied resource files
│   └── Makefile
├── CMakeLists.txt
├── README.md
├── ARCHITECTURE.md
├── SETUP.md                     # This file
├── build.sh
├── include/
└── src/
```

## What You Should See

When you run the game, you should see:
1. **Console output**: "Game initialized successfully!"
2. **Window**: A dark blue window (1280x720) titled "City Builder - Demacia Rising Style"
3. **Controls**: Press ESC to close the window

The window is currently blank - that's expected! We haven't added rendering yet.

## Next Development Steps

Now that the foundation is working, here's what to implement next:

### 1. Basic Rendering (Recommended First Step)
- Create GameMap class with initial nodes
- Render nodes as circles on screen
- Render connection lines between nodes
- Add node labels

**Files to create:**
- `include/Map.h`
- `src/Map.cpp`

### 2. UI Rendering
- Display resources (gold, food, wood) in top bar
- Show turn number
- Show action points remaining

**Files to create:**
- `include/UIRenderer.h`
- `src/UIRenderer.cpp`

### 3. Mouse Interaction
- Detect mouse position
- Highlight node under cursor
- Click to select nodes
- Show node info panel

### 4. Building System
- Add building placement UI
- Handle resource spending
- Update node visuals with building icons

### 5. Turn System
- Implement "End Turn" button
- Resource generation
- Action point reset

## Recommended Learning Resources

If you're new to SFML:
- **Official Tutorial**: https://www.sfml-dev.org/tutorials/2.6/
- **Game Loop**: Start with "Window" and "Graphics" tutorials
- **Events**: Learn about `sf::Event` handling

If you're new to C++ game development:
- **Game Programming Patterns**: https://gameprogrammingpatterns.com/
- Focus on: Game Loop, Update Method, Component Pattern

## Development Workflow

1. **Make changes** to .cpp or .h files
2. **Rebuild**: Run `./build.sh` or `make` in build directory
3. **Test**: Run `./build/CityBuilderGame`
4. **Debug**: Add `std::cout` statements or use gdb/lldb
5. **Iterate**: Repeat!

### Quick Rebuild
```bash
# From project root
cd build && make && cd .. && ./build/CityBuilderGame
```

Or create an alias:
```bash
alias rungame='cd build && make && ./CityBuilderGame'
```

## Common Build Commands

```bash
# Full clean rebuild
rm -rf build && mkdir build && cd build && cmake .. && make

# Just recompile changes
cd build && make

# Verbose build (see all commands)
cd build && make VERBOSE=1

# Build with debug symbols
cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

# Build optimized release
cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make
```

## Getting Help

If you get stuck:
1. Check the error message carefully
2. Look at ARCHITECTURE.md for design explanation
3. Read SFML documentation for specific features
4. Check that all files are in the right directories
5. Make sure CMakeLists.txt includes all new .cpp files

## Success Checklist

- [ ] SFML installed and found by CMake
- [ ] Project compiles without errors
- [ ] Game window opens (dark blue, 1280x720)
- [ ] Console shows "Game initialized successfully!"
- [ ] Window closes when pressing ESC
- [ ] Ready to add game features!

Once all these are checked, you're ready to start implementing the game logic!
