#include <bitset>
#include "../../include/typedefs.h"

enum StatusRegisterFlags
{
    CARRY,
    ZERO,
    INTERRUPT_REQUEST,
    DECIMAL_MODE,
    BREAK_COMMAND,
    UNUSED,
    OVERFLOW,
    NEGATIVE
};

class CPURegisters
{
public:
    WORD program_counter{};
    BYTE accumulator{};
    BYTE X{};   
    BYTE Y{};
    BYTE stack_pointer{};
    std::bitset<8> status_register{};

    void CheckZero(BYTE val);
    void CheckNegative(BYTE val);
    void CheckCarry(WORD val);
    void CheckCarryCompare(BYTE regVal, BYTE val) ;
};