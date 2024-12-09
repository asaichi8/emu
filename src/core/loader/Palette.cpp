#include "Palette.h"


Palette::Palette()
{
    m_curPalette.assign(COLOUR_PALETTE_SIZE, {0, 0, 0});
}


// https://www.nesdev.org/wiki/PPU_palettes#Palettes

/// @brief 
/// @param filePath 
/// @return True if success, false if otherwise 
bool Palette::LoadPalette(const std::string& filePath)
{
    auto rawFile = Loader::LoadFile(filePath);

    if (rawFile.size() < (COLOUR_PALETTE_SIZE * sizeof(RGB)))
    {
        std::cerr << "ERROR: Failed to load palette, size was too small!" << std::endl;
        return false;
    }

    const RGB* paletteData = (const RGB*)(rawFile.data());
    m_curPalette.assign(paletteData, paletteData + COLOUR_PALETTE_SIZE);
    
    return true;
}