cmake_minimum_required(VERSION 3.16)
project(CityBuilderGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Capitalize component names for SFML 3
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES src/main.cpp src/Game.cpp src/Map.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    sfml-graphics 
    sfml-window 
    sfml-system
)