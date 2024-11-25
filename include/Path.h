#pragma once

#include <string>

constexpr char RELATIVE_PATH[] = "../bin/snake.nes";
//constexpr char RELATIVE_PATH[] = "../bin/nestest2.nes";

static std::string GetROMPath()
{
    std::string currentPath = __FILE__;
    size_t lastSlashPos = currentPath.find_last_of('/');
    std::string path = currentPath.substr(0, lastSlashPos + 1) + RELATIVE_PATH;
    
    return path;
}