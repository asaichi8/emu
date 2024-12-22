#include "ConfigQueue.h"


ConfigQueue::ConfigQueue(const std::string& configName, size_t queueSize) : m_strConfigName(configName), m_nQueueSize(queueSize)
{
    if (configName.empty() || m_nQueueSize == 0)
        throw std::invalid_argument("Arguments were invalid!");
        
    Setup();
}


void ConfigQueue::Setup()
{
    Config* config = &Config::GetInstance();
    
    // if we don't have a file, or we don't have the recent files header, make a file/generate the header
	if (!config->_file.read(config->ini) || !config->ini.has(m_strConfigName))
    {
        MakeConfig(m_strConfigName);
        return;
    }
	
    // for each member of the config file under the recent files header...
	for (const auto& [key, val] : config->ini[m_strConfigName])
	{
        if (val.empty())
            continue;

		if (m_recentFiles.size() >= m_nQueueSize) // make sure we don't overflow
			m_recentFiles.pop_front();
		
		m_recentFiles.push_back(val); // save the value
	}
}

void ConfigQueue::MakeConfig(const std::string& configName)
{
    Config* config = &Config::GetInstance();

    config->_file.read(config->ini);
    for (int i = 1; i <= m_nQueueSize; ++i)
    {
        std::string val = "";
        if (m_recentFiles.size() > (i - 1))
            val = m_recentFiles.at(i - 1);

        config->ini[configName]["file" + std::to_string(i)] = val;
    }

    if (!config->_file.write(config->ini, true))
        std::cerr << "Failed to write to config file!" << std::endl;
}

void ConfigQueue::Push(std::string val, bool writeToFile)
{
    if (val.empty())
        return;

    // if duplicate, reuse the string and place it at the front
    auto it = std::find(m_recentFiles.begin(), m_recentFiles.end(), val);
    if (it != m_recentFiles.end())
    {
        m_recentFiles.erase(it);
        m_recentFiles.push_front(val);
    }
    else // not a duplicate
    {
        if (m_recentFiles.size() >= m_nQueueSize)
            m_recentFiles.pop_back();
        
        m_recentFiles.push_front(val);
    }

    if (writeToFile)
        MakeConfig(m_strConfigName);
}