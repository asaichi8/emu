#include "Palette.h"


Palette::Palette()
{
    curPalette.assign(PALETTE_SIZE, {0, 0, 0});
}


void Palette::LoadPalette(const std::string& filePath)
{
    auto rawFile = LoadFile(filePath);
}