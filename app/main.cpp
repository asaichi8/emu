#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdint>
#include "../core/cpu/CPU.h"

#define KB 1024

/*std::vector<BYTE> HexStrToBytes(const std::string& hexString) 
{
    std::vector<BYTE> bytes;
    std::istringstream stream(hexString);
    std::string byteStr;

    while (stream >> byteStr)
    {
        WORD w;
        std::stringstream hexStream(byteStr);
        hexStream >> std::hex >> w;
        bytes.push_back((BYTE)w);
    }

    return bytes;
}

std::vector<BYTE> LoadFile(const std::string& filePath) 
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) 
    {
        std::cerr << "err1" << std::endl;
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<BYTE> buffer(size);
    if (!file.read((char*)(buffer.data()), size)) 
    {
        std::cerr << "err2" << std::endl;
        return {};
    }

    return buffer;
}*/

int main()
{
    constexpr size_t CPU_RAM_SIZE = 64*KB;
    constexpr WORD START_ADDR = 0xC000;
    RAM ram(CPU_RAM_SIZE);
    ram.WriteWord(CPU::RESET_VECTOR, START_ADDR); // start the program at specific mem location
    CPU cpu(&ram);
    
    // example code
    /*std::string assem = "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
    auto bytes = LoadFile("/home/---/Downloads/test.bin");
    auto bytes2 = HexStrToBytes(assem);

    for (int i = 0; i < bytes.size(); ++i)
    {
        ram.WriteByte((WORD)(i), bytes.at(i));
    }*/

    cpu.Run();

    return 0;
}