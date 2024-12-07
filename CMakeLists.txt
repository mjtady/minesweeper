cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(minesweeper)

## If you want to link SFML statically
# set(SFML_STATIC_LIBRARIES TRUE)

## In most cases better set in the CMake cache
set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)
add_executable(minesweeper src/main.cpp
        src/welcome.h
        src/welcome.cpp
        src/game.cpp
        src/game.h
        src/leaderboard.h
        src/leaderboard.cpp
)
target_link_libraries(minesweeper sfml-system sfml-window sfml-graphics sfml-audio)