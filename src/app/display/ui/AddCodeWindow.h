#pragma once


#include <sstream>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include "IGUIWindow.h"
#include "../../../core/cheats/GameGenie.h"
#include "../../../core/loader/Loader.h"
#include "../../../core/loader/DatabaseHandler.h"


class AddCodeWindow : public IGUIWindow
{
    char m_szAddress[5]{};
    char m_szValue[3]{};
    char m_szCompare[3]{};

    char m_szCode[9]{};
    char m_szDescription[256]{};

    // TODO: this is already definied in Emulator.h & CodeListWindow.h, we shouldnt do this multiple times
    const std::string DATABASE_RELATIVE_PATH = "../bin/rom_database.json";

    Loader::GameInfo** m_ppGameInfo{};
    std::pair<std::string, std::string>** m_ppMD5pair{};
    std::mutex* m_pDBmutex;


    static bool IsValidGameGenieChar(char c);
    void CreateEntry(const std::string& entryName, char* buffer, size_t bufSize, ImGuiInputTextFlags flags = 0, const std::function<void()>& callback = nullptr);
    bool AttemptEncode();
    bool AttemptDecode();

public:
    AddCodeWindow(const std::string& name, Loader::GameInfo** pGameInfo, std::pair<std::string, std::string>** pMD5pair, std::mutex* dbMutex);

    void Draw() override;
};