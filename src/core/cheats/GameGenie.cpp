#include "GameGenie.h"


GameGenie::GameGenie()
{

}


// assumes code is of length 6 or 8
GameGenie::DecodedCode GameGenie::Decode(std::string code)
{
    const size_t numBits = code.length() * 4;

    std::vector<int> positionMap{};
    if (numBits == 24)
        positionMap = POSITION_MAP_6;
    else if (numBits == 32)
        positionMap = POSITION_MAP_8;
    else
        return {}; // TODO: this should throw an error
        //throw std::invalid_argument("String length was not 6 or 8!");
    
    GameGenie::DecodedCode decodedCode{};
    std::transform(code.begin(), code.end(), code.begin(), ::toupper);

    // e.g. code was SXIOPO, so encodedBits is 1101 1010 0101 1001 0001 1001 (leftmost position is 23)
    std::vector<bool> encodedBits = StringCodeToBits(code);
    std::vector<bool> decodedBits(numBits, 0);
    for (int i = 0; i < numBits; ++i)
    {
        if (positionMap.at(i) == -1)
        {
            decodedBits[(numBits - 1) - i] = 0;
            continue;
        }

        decodedBits[(numBits - 1) - positionMap.at(i)] = encodedBits[(numBits - 1) - i];
    }
    

    // now decodedBits is 1010 1101 0001 0001 1101 1001 (leftmost position is 23)

    // first 8 bits (refers to the left hand side (1010 1101)), evaluates to the value
    std::vector<bool> bitsValue = std::vector<bool>(decodedBits.end() - 8, decodedBits.end()); // 1010 1101

    // skip 1 and get next 15 bits, which evaluates to the address
    std::vector<bool> bitsAddress = std::vector<bool>(decodedBits.end() - 24, decodedBits.end() - 9); // 001 0001 1101 1001

    // if the string was of 8 length, get the compare address (last 8 bits)
    std::vector<bool> bitsCompare{};
    if (decodedBits.size() >= 32)
        bitsCompare = std::vector<bool>(decodedBits.end() - 32, decodedBits.end() - 24);
        

    auto BitsToInt = [](const std::vector<bool>& bits) -> int { // processes bits from LEFT TO RIGHT
        int val = 0;
        for (size_t i = 0; i < bits.size(); ++i)
        {
            val |= bits[i] << i; // process left to right
        }
        return val;
    };

    decodedCode.val = BitsToInt(bitsValue); // extract high 8 bits
    decodedCode.addr = BitsToInt(bitsAddress); // ignore 9th bit and extract next 15 bits
    if (decodedBits.size() >= 32)
        decodedCode.compare = BitsToInt(bitsCompare); // extract low 8 bits

    return decodedCode;
}

// the exact opposite of decode
std::string GameGenie::Encode(const GameGenie::DecodedCode& decodedCode)
{
    const size_t numBits = decodedCode.compare.has_value() ? 32 : 24;
    const std::vector<int>& positionMap = (numBits == 24) ? POSITION_MAP_6 : POSITION_MAP_8;

    std::string code{};


    std::vector<bool> decodedBits(numBits, 0);
    // VAL : set each bit (decodedBits[23] to decodedBits[23 - 8] to the appropriate value)
    for (int i = 0; i < 8; ++i)
    {
        decodedBits[numBits - 8 + i] = (decodedCode.val >> i) & 1;
    }

    // ADDR : 
    for (int i = 0; i < 15; ++i)
    {
        decodedBits[numBits - 24 + i] = (decodedCode.addr >> i) & 1;
    }

    // COMPARE : 
    if (decodedCode.compare.has_value())
    {
        for (int i = 0; i < 8; ++i)
        {
            decodedBits[numBits - 32 + i] = (decodedCode.compare.value_or(0) >> i) & 1;
        }
    }


    std::vector<bool> encodedBits(numBits, 0);
    for (size_t i = 0; i < numBits; ++i)
    {
        if (positionMap.at(i) == -1)
        {
            // this is the spare bit in the address value. it does not matter whether it is set to 1 or 0, but in most of the official
            // game genie codes i've seen, it's set to 1 when there is a compare value - so we'll just do that
            encodedBits[(numBits - 1) - i] = decodedCode.compare.has_value() ? 1 : 0;
            continue;
        }

        encodedBits[(numBits - 1) - i] = decodedBits[(numBits - 1) - positionMap.at(i)];
    }

    
    return BitCodeToString(encodedBits);
}


// Maps each character to its equivalent hex value in the hex table, and returns a vector<bool> as a bitset.
// e.g. code is SXIOPO, return value would be:
// 1101 1010 0101 1001 0001 1001
// where bits[23] is the left side, and bits[0] is the right side (bits[22] == 1, bits[1] == 0)
std::vector<bool> GameGenie::StringCodeToBits(const std::string& code)
{
    if (code.empty())
        throw std::invalid_argument("String length must be greater than 0!");

    const size_t numBits = code.length() * 4;
    std::vector<bool> bits(numBits, 0);

    for (int i = 0; i < code.length(); ++i)
    {
        char c = code.at(i);

        auto it = HexTable.find(c);
        if (it == HexTable.end())
            throw std::invalid_argument("Not a valid Game Genie code!");
        
        size_t shiftVal = numBits - ((i + 1) * 4); // hex values go from left to right rather than right to left
        
        for (int j = 3; j >= 0; --j)
        {
            bits.at(shiftVal + j) = (it->second >> j) & 1;
        }
    }

    return bits;
}

std::string GameGenie::BitCodeToString(const std::vector<bool>& code)
{
    const size_t numBits = code.size();
    if (numBits == 0 || (numBits % 4 != 0))
        throw std::invalid_argument("Bit length must be a multiple of 4!");
    
    const size_t numChars = numBits / 4;
    std::string strCode(numChars, ' ');

    for (size_t i = 0; i < numChars; ++i)
    {
        int nibbleVal{};
        
        size_t shiftVal = numBits - ((i + 1) * 4);
        for (int j = 3; j >= 0; --j)
        {
            nibbleVal |= (code.at(shiftVal + j) & 1) << j;
        }

        for (auto const& pair : HexTable)
        {
            if (pair.second == nibbleVal)
            {
                strCode.at(i) = pair.first;
                break;
            }
        }
    }

    return strCode;
}


std::vector<std::string> GameGenie::ExtractCodes(const std::string& codes, char delimiter)
{
    std::vector<std::string> result{};
    std::string token{};

    std::istringstream stream(codes);
    while (std::getline(stream, token, delimiter))
    {
        // remove whitespace
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");

        if (start == std::string::npos || end == std::string::npos)
            continue;

        result.push_back(token.substr(start, end - start + 1));
    }

    return result;
}

std::string GameGenie::ConcatenateCodes(const std::vector<std::string>& codes, std::string delimiter)
{
    std::string s{};

    for (int i = 0; i < codes.size(); ++i)
    {
        s += codes.at(i);

        if (i != codes.size() - 1) // if not last element of array
            s += delimiter;
    }

    return s;
}
