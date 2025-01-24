#include <iostream>
#include "yaml-cpp/parser.h"
#include "yaml-cpp/yaml.h"
#include "Library/Setting/CSetting.h"

CSetting::CSetting(const std::string InFileName)
{    
    YAML::Node config = YAML::LoadFile(InFileName);

    port = config["port"].as<int>();
}

int CSetting::GetPort() const
{
    return this->port;
}
