#pragma once

#include <string>
#include <deque>
#include <iostream>
#include "../include/Config.h"

class RecentFileQueue
{
    static const size_t QUEUE_SIZE = 10;
	static const inline std::string STR_RECENT_FILES = "recentfiles";
    std::deque<std::string> m_recentFiles;

    void Setup();
    void MakeConfig(const std::string& configName);

public:
    RecentFileQueue();

    void Push(std::string val, bool writeToFile = true);
    const std::deque<std::string>& GetQueue() const { return m_recentFiles; }
};