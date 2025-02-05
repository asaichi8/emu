#pragma once

#include "lib/mINI/ini.h"


// singleton
class Config
{
    Config() : _file("config.ini") {};
    ~Config(){};

public:
    mINI::INIFile _file;
    mINI::INIStructure ini;

    static Config& GetInstance()
    {
        static Config config;
        return config;
    }

    // prevent copying
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};