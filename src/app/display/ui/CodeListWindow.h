#pragma once

#include <vector>
#include "IGUIWindow.h"
#include "../../../core/loader/Loader.h"


class CodeListWindow : public IGUIWindow
{
    Loader::GameInfo** m_ppGameInfo{};

public:
    CodeListWindow(const std::string& name, Loader::GameInfo** pGameInfo);

    void Draw() override;
};