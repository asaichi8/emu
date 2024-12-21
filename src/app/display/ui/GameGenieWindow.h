#pragma once

#include <vector>
#include "IGUIWindow.h"
#include "../../../core/loader/Loader.h"


class GameGenieWindow : public IGUIWindow
{
    std::vector<GameGenie::GameGenieCode>** m_ppCodes{};

public:
    GameGenieWindow(const std::string& name, std::vector<GameGenie::GameGenieCode>** pCodes);

    void Draw() override;
};