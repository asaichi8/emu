#pragma once

#include <cstdint>

#define BYTE    uint8_t 
#define WORD    uint16_t
#define DWORD   uint32_t
#define QWORD   uint64_t

#define PAGE    0x00FF

#define STACK   0x0100 // beginning of stack (0x0100 - 0x01FF)
