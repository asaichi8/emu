#include "RecentFileQueue.h"



RecentFileQueue::RecentFileQueue()
{
    Setup();
}


void RecentFileQueue::Setup()
{
    Config* config = &Config::GetInstance();
    
    // if we don't have a file, or we don't have the recent files header, make a file/generate the header
	if (!config->_file.read(config->ini) || !config->ini.has(STR_RECENT_FILES))
    {
        MakeConfig(STR_RECENT_FILES);
        return;
    }
	
    // for each member of the config file under the recent files header...
	for (const auto& [key, val] : config->ini[STR_RECENT_FILES])
	{
        if (val.empty())
            continue;

		if (m_recentFiles.size() >= QUEUE_SIZE) // make sure we don't overflow
			m_recentFiles.pop_front();
		
		m_recentFiles.push_back(val); // save the value
	}
}

void RecentFileQueue::MakeConfig(const std::string& configName)
{
    Config* config = &Config::GetInstance();

    config->_file.read(config->ini);
    for (int i = 1; i <= 5; ++i)
    {
        std::string val = "";
        if (m_recentFiles.size() > (i - 1))
            val = m_recentFiles.at(i - 1);

        config->ini[configName]["file" + std::to_string(i)] = val;
    }

    if (!config->_file.write(config->ini, true))
        std::cerr << "Failed to write to config file!" << std::endl;
}

void RecentFileQueue::Push(std::string val, bool writeToFile)
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
        if (m_recentFiles.size() >= QUEUE_SIZE)
            m_recentFiles.pop_back();
        
        m_recentFiles.push_front(val);
    }

    if (writeToFile)
        MakeConfig(STR_RECENT_FILES);
}