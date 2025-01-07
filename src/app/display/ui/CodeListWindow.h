#pragma once

#include <vector>
#include "IGUIWindow.h"
#include "../../../core/loader/Loader.h"
#include "../../../core/loader/DatabaseHandler.h"


class CodeListWindow : public IGUIWindow
{
    // TODO: this is already definied in Emulator.h & AddCodeWindow.h, we shouldnt do this multiple times
    const std::string DATABASE_RELATIVE_PATH = "../bin/rom_database.json";

    Loader::GameInfo** m_ppGameInfo{};
    std::pair<std::string, std::string>** m_ppMD5pair{};

public:
    CodeListWindow(const std::string& name, Loader::GameInfo** pGameInfo, std::pair<std::string, std::string>** pMD5pair);

    void Draw() override;
};