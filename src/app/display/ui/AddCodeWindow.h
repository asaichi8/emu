#pragma once


//#include <vector>
#include "IGUIWindow.h"
//#include "../../../core/loader/Loader.h"


class AddCodeWindow : public IGUIWindow
{
    char m_szAddress[5]{};
    char m_szValue[3]{};
    char m_szCompare[3]{};

public:
    AddCodeWindow(const std::string& name);

    void CreateEntry(const std::string& entryName, char* buffer, size_t bufSize);
    void Draw() override;
};