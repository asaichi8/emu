#include "Joypad.h"


Joypad::Joypad()
{
    
}

Joypad::~Joypad()
{

}


// https://www.nesdev.org/wiki/Standard_controller#Input_($4016_write)
// CPU write
void Joypad::CPUWrite(BYTE val)
{
    m_bStrobe = val & 0b1;
    if (m_bStrobe)
        m_Button = 0;
}

// https://www.nesdev.org/wiki/Standard_controller#Output_($4016/$4017_read)
std::bitset<8> Joypad::CPURead()
{
    if (m_Button > 7)
        return 1; // "All subsequent reads will return 1"    

    BYTE button = m_joypadState.test(m_Button);
    if (!m_bStrobe)
        m_Button++;

    return button;
}

void Joypad::Update(Button button, bool isDown)
{
    m_joypadState.set(button, isDown);
}
