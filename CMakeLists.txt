cmake_minimum_required(VERSION 3.10)
project(emuproj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_REENTRANT -Wno-pragma-pack -pthread")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_SOURCE_DIR}/src/app/display
    ${CMAKE_SOURCE_DIR}/src/app/display/SDL
    ${CMAKE_SOURCE_DIR}/src/app/display/SDL/controllers
    ${CMAKE_SOURCE_DIR}/src/core/cpu
    ${CMAKE_SOURCE_DIR}/src/core/ppu
    ${CMAKE_SOURCE_DIR}/src/core/ppu/registers
    ${CMAKE_SOURCE_DIR}/src/core/apu
    ${CMAKE_SOURCE_DIR}/src/core/controller
    ${CMAKE_SOURCE_DIR}/src/core/loader
    ${CMAKE_SOURCE_DIR}/src/core/memory
    ${CMAKE_SOURCE_DIR}/src/demos
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/src/include/imgui
    ${CMAKE_SOURCE_DIR}/src/include/ini
    ${CMAKE_SOURCE_DIR}/src/include/tinyfiledialogs
)

file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/cpu/*.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ppu/*.cpp
    ${CMAKE_SOURCE_DIR}/src/core/memory/*.cpp
    ${CMAKE_SOURCE_DIR}/src/core/loader/*.cpp
    ${CMAKE_SOURCE_DIR}/src/core/controller/*.cpp
    ${CMAKE_SOURCE_DIR}/src/app/*.cpp
    ${CMAKE_SOURCE_DIR}/src/app/display/*.cpp
    ${CMAKE_SOURCE_DIR}/src/app/display/SDL/*.cpp
    ${CMAKE_SOURCE_DIR}/src/app/display/SDL/controllers/*.cpp
    ${CMAKE_SOURCE_DIR}/src/demos/*.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/*.cpp
    ${CMAKE_SOURCE_DIR}/src/include/tinyfiledialogs/*.c
)

add_executable(emu ${SOURCES})

find_package(SDL2 REQUIRED)
if (SDL2_VERSION VERSION_LESS "2.0.17")
    message(FATAL_ERROR "SDL2 version 2.0.17 or higher is required.")
endif()
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(emu ${SDL2_LIBRARIES})
