#include <iostream>
#include "../core/cpu/CPU.h"


int main()
{
    RAM ram;
    CPU cpu(&ram);
    std::cout << "hello world" << std::endl;

    return 0;
}