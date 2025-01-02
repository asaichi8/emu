#pragma once


//#include <vector>
#include <functional>
#include "IGUIWindow.h"
#include "../../../core/cheats/GameGenie.h"


class AddCodeWindow : public IGUIWindow
{
    char m_szAddress[5]{};
    char m_szValue[3]{};
    char m_szCompare[3]{};

    char m_szCode[9]{};

    static bool IsValidGameGenieChar(char c);
    void CreateEntry(const std::string& entryName, char* buffer, size_t bufSize, ImGuiInputTextFlags flags = 0, const std::function<void()>& callback = nullptr);
    bool AttemptEncode();

public:
    AddCodeWindow(const std::string& name);

    void Draw() override;
};