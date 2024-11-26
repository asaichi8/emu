#include "PPURegisters.h"

PPURegisters::PPURegisters()
{

}


bool PPURegisters::IsReadable(Registers reg)
{
    return (reg == Registers::PPUSTATUS || reg == Registers::OAMDATA || reg == Registers::PPUDATA);
}

bool PPURegisters::IsWritable(Registers reg)
{
    return reg != PPUSTATUS;
}
