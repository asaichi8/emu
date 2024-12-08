cmake_minimum_required(VERSION 3.10)
project(emuproj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_REENTRANT")

include_directories(
    ${CMAKE_SOURCE_DIR}/app
    ${CMAKE_SOURCE_DIR}/app/display
    ${CMAKE_SOURCE_DIR}/app/display/SDL
    ${CMAKE_SOURCE_DIR}/core/cpu
    ${CMAKE_SOURCE_DIR}/core/ppu
    ${CMAKE_SOURCE_DIR}/core/ppu/registers
    ${CMAKE_SOURCE_DIR}/core/apu
    ${CMAKE_SOURCE_DIR}/core/loader
    ${CMAKE_SOURCE_DIR}/core/memory
    ${CMAKE_SOURCE_DIR}/demos
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/imgui
)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/core/cpu/*.cpp
    ${CMAKE_SOURCE_DIR}/core/ppu/*.cpp
    ${CMAKE_SOURCE_DIR}/core/memory/*.cpp
    ${CMAKE_SOURCE_DIR}/core/loader/*.cpp
    ${CMAKE_SOURCE_DIR}/app/*.cpp
    ${CMAKE_SOURCE_DIR}/app/display/*.cpp
    ${CMAKE_SOURCE_DIR}/app/display/SDL/*.cpp
    ${CMAKE_SOURCE_DIR}/demos/*.cpp
    ${CMAKE_SOURCE_DIR}/imgui/*.cpp
)

add_executable(emu ${SOURCES})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(emu ${SDL2_LIBRARIES})