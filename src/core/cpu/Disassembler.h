#pragma once

#include <unordered_map>
#include <type_traits>
#include "../memory/Bus.h"


class CPU;

class Disassembler
{
public:
    struct DisasmInfo
    {
        std::string instruction{}; // opcode + operand string
        size_t size{};             // size of instruction
        std::string addrMode{};    // string denoting addressing mode
    };

private:
	std::shared_ptr<Bus> m_Bus{};

    // SFINAE: only compile templated functions that are of integral type (e.g. BYTE, WORD etc.)
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    std::string ValToHexStr(T val);

    // handler functions operate on a DisasmInfo pointer and take an addr that the bus will read from to get the operand
#define INSTRUCTION_HANDLER(handlerName) void handlerName(DisasmInfo* info, WORD addr)
    INSTRUCTION_HANDLER(HandleIMP);
    INSTRUCTION_HANDLER(HandleACC);
    INSTRUCTION_HANDLER(HandleIMM);
    INSTRUCTION_HANDLER(HandleABS);
    INSTRUCTION_HANDLER(HandleABX);
    INSTRUCTION_HANDLER(HandleABY);
    INSTRUCTION_HANDLER(HandleIND);
    INSTRUCTION_HANDLER(HandleREL);
    INSTRUCTION_HANDLER(HandleZPG);
    INSTRUCTION_HANDLER(HandleZPX);
    INSTRUCTION_HANDLER(HandleZPY);
    INSTRUCTION_HANDLER(HandleIZY);
    INSTRUCTION_HANDLER(HandleIZX);

	// string denoting the addressing mode (e.g. "IMM" == &CPU::IMM), followed by a function pointer pointing to a
	// lambda function that captures the current instance of this class and calls the accompanying addressing mode handler
	// (e.g. "IMM" is accompanied by this->HandleIMM(info, addr); )
#define HANDLE_PAIR(addrMode) {#addrMode, [this](DisasmInfo* info, WORD addr) { Handle##addrMode(info, addr); }}
	const std::unordered_map<std::string, std::function<void(DisasmInfo*, WORD)>> addrModeHandlers =
	{
		HANDLE_PAIR(IMP),
		HANDLE_PAIR(ACC),
		HANDLE_PAIR(IMM),
		HANDLE_PAIR(ABS),
		HANDLE_PAIR(ABX),
		HANDLE_PAIR(ABY),
		HANDLE_PAIR(IND),
		HANDLE_PAIR(REL),
		HANDLE_PAIR(ZPG),
		HANDLE_PAIR(ZPX),
		HANDLE_PAIR(ZPY),
		HANDLE_PAIR(IZY),
		HANDLE_PAIR(IZX)
	};

public:
    Disassembler(std::shared_ptr<Bus> bus);

    DisasmInfo Disassemble(WORD addr);
};