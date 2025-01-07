#include "AddCodeWindow.h"


AddCodeWindow::AddCodeWindow(const std::string& name, Loader::GameInfo** pGameInfo, std::pair<std::string, std::string>** pMD5pair)
    : IGUIWindow(name), m_ppGameInfo(pGameInfo), m_ppMD5pair(pMD5pair)
{

}


void AddCodeWindow::Draw()
{
    ImGui::SetNextWindowSizeConstraints({0.f, 0.f}, {750.f, 500.f});
    ImGui::Begin("Add Game Genie Code", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    
    // IM_ARRAYSIZE - "Don't use on pointers!", so pass it here instead
    // call AttemptEncode() every time an entry is modified
    CreateEntry("Address:   0x", m_szAddress, IM_ARRAYSIZE(m_szAddress), ImGuiInputTextFlags_CharsHexadecimal, [&]() { AttemptEncode(); });
    CreateEntry("Value:     0x", m_szValue,   IM_ARRAYSIZE(m_szValue)  , ImGuiInputTextFlags_CharsHexadecimal, [&]() { AttemptEncode(); });
    CreateEntry("Compare:   0x", m_szCompare, IM_ARRAYSIZE(m_szCompare), ImGuiInputTextFlags_CharsHexadecimal, [&]() { AttemptEncode(); });

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Code:        ");
    ImGui::SameLine();
    // ensure that the only text in this InputText can be a valid game genie code character
    if (ImGui::InputText("##Code", m_szCode, IM_ARRAYSIZE(m_szCode), ImGuiInputTextFlags_CallbackCharFilter,
                     [](ImGuiInputTextCallbackData* data) -> int { return !IsValidGameGenieChar(data->EventChar); }))
    {
        AttemptDecode(); // call AttemptDecode() every time the code is modified
    }

    CreateEntry("Description: ", m_szDescription, IM_ARRAYSIZE(m_szDescription));
        

    ImGui::Spacing();

    ImGui::BeginDisabled(strlen(m_szCode) != 6 && strlen(m_szCode) != 8); // disable button if invalid code
    if (ImGui::Button("Add to code list", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    {
        // create our code as a GameGenieCode
        GameGenie::GameGenieCode curCode = {{m_szCode}, m_szDescription, false};

        // add code to the list, updating the views
        auto pGameInfo = *m_ppGameInfo;
        pGameInfo->gameGenieCodes.push_back(curCode);

        // attempt to insert new code into database
        auto pMD5pair = *m_ppMD5pair;
        if (!DatabaseHandler::InsertInfoW(**m_ppGameInfo, pMD5pair->first, pMD5pair->second, Loader::GetFullFilePath(DATABASE_RELATIVE_PATH), false))
            std::cerr << "failed to insert info" << std::endl;
    }
    ImGui::EndDisabled();

    ImGui::End();
}


void AddCodeWindow::CreateEntry(const std::string& entryName, char* buffer, size_t bufSize, ImGuiInputTextFlags flags, const std::function<void()>& callback)
{
    ImGui::Text("%s", entryName.c_str());
    ImGui::SameLine();

    std::string textboxName = "##" + entryName;
    if (ImGui::InputText(textboxName.c_str(), buffer, bufSize, flags))
    {
        // use a callback so that we can keep this function modular
        if (callback)
            callback();
    }
}

bool AddCodeWindow::IsValidGameGenieChar(char c)
{
    return GameGenie::HexTable.find(toupper(c)) != GameGenie::HexTable.end();
}

bool AddCodeWindow::AttemptEncode()
{
    if (strlen(m_szAddress) == 0 || strlen(m_szValue) == 0)
        return false;

    GameGenie::DecodedCode code{};
    code.addr = std::stoi(m_szAddress, nullptr, 16);
    code.val = std::stoi(m_szValue, nullptr, 16);
    if (strlen(m_szCompare) != 0)
        code.compare = std::stoi(m_szCompare, nullptr, 16);
    
    std::string encodedString = GameGenie::Encode(code);
    if (encodedString.length() != 6 && encodedString.length() != 8)
    {
        std::cerr << "Encoded string was wrong length: " << encodedString.length() << std::endl;
        return false;
    }

    strncpy(m_szCode, encodedString.c_str(), sizeof(m_szCode));

    return true;
}

// assumes that the code in m_szCode has valid game genie code characters!
bool AddCodeWindow::AttemptDecode()
{
    if (!(strlen(m_szCode) == 6 || strlen(m_szCode) == 8))
        return false;
    
    GameGenie::DecodedCode code = GameGenie::Decode(m_szCode);

    std::stringstream ss;

    ss << std::hex << std::uppercase << code.addr;
    strncpy(m_szAddress, ss.str().c_str(), sizeof(m_szAddress));

    ss.str(""); ss.clear();
    ss << std::hex << std::uppercase << (int)code.val;
    strncpy(m_szValue, ss.str().c_str(), sizeof(m_szValue));

    if (code.compare.has_value())
    {
        ss.str(""); ss.clear();
        ss << std::hex << std::uppercase << (int)code.compare.value();
        strncpy(m_szCompare, ss.str().c_str(), sizeof(m_szCompare));
    }
    else
    {
        m_szCompare[0] = '\0'; // clear string
    }

    return true;
}