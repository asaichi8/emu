#pragma once

#include <cstdint>

#define CHAR    int8_t
#define BYTE    uint8_t 
#define WORD    uint16_t
#define DWORD   uint32_t
#define QWORD   uint64_t

#define CHAR_MAX 0x7F
#define BYTE_MAX UINT8_MAX
#define WORD_MAX UINT16_MAX
#define DWORD_MAX UINT32_MAX
#define QWORD_MAX UINT64_MAX

#define PAGE    0x0100

#define STACK   0x0100 // beginning of stack (0x0100 - 0x01FF)
