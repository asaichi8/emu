#pragma once

#include <vector>
#include "IGUIWindow.h"


class ErrorMsgWindow : public IGUIWindow
{
    std::string m_lastErrorTitle = "Error";
    std::string m_lastError = "Unknown error occured!";

public:
    ErrorMsgWindow(const std::string& name);

    void Draw() override;
    void SetError(const std::string& title, const std::string& error);
};