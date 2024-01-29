#include "Loader.h"

Loader::Loader()
{

}

std::vector<BYTE> Loader::HexStrToBytes(const std::string& hexString) 
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

std::vector<BYTE> Loader::LoadFile(const std::string& filePath) 
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
}