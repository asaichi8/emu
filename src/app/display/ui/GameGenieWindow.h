#pragma once

#include <vector>
#include "IGUIWindow.h"
#include "../../../core/loader/Loader.h"


class GameGenieWindow : public IGUIWindow
{
    Loader::GameInfo** m_ppGameInfo{};

public:
    GameGenieWindow(const std::string& name, Loader::GameInfo** pGameInfo);

    void Draw() override;
};