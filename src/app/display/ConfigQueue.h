#pragma once

#include <string>
#include <deque>
#include "pch.h"
#include "../include/Config.h"


class ConfigQueue
{
    size_t m_nQueueSize = 10;
	std::string m_strConfigName = "recentfiles";
    std::deque<std::string> m_recentFiles;

    void Setup();
    void MakeConfig(const std::string& configName);

public:
    ConfigQueue(const std::string& configName, size_t queueSize);

    void Push(std::string val, bool writeToFile = true);
    const std::deque<std::string>& GetQueue() const { return m_recentFiles; }
};