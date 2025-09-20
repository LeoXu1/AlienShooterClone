cmake_minimum_required(VERSION 3.30)

project(AlienShooterClone)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(game space_invaders.cpp GameObject.cpp GameObject.h InvaderManager.cpp InvaderManager.h Player.cpp Player.h space_invaders_common.h)

target_link_libraries(game SDL2::SDL2 SDL2::SDL2main SDL2_ttf::SDL2_ttf)

