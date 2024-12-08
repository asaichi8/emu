#pragma once

#include <bitset>
#include "../../include/typedefs.h"

class Joypad
{
    BYTE m_Button{};
    std::bitset<8> m_joypadState{};
    bool m_bStrobe = false;

public:
    Joypad();
    ~Joypad();

    // https://www.nesdev.org/wiki/Standard_controller#Report
    enum Button
    {
        A,
        B,
        SELECT,
        START,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    void CPUWrite(BYTE val);
    std::bitset<8> CPURead();
    void Update(Button button, bool isDown);
};