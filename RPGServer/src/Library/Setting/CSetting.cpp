#include <iostream>
#include <filesystem>

#include "Library/Setting/CSetting.h"

#include "yaml-cpp/parser.h"
#include "yaml-cpp/yaml.h"


void CSetting::LoadSettings(const std::string InFileName)
{
    if (!std::filesystem::exists(InFileName)) {
        std::cerr << "Error : not found yaml\n ";
        return;
    }

    YAML::Node config = YAML::LoadFile(InFileName);

    std::cout << "YAML Loaded:\n" << config << std::endl;

    serverPort = config["port"].as<int>();

    // redis check
    if (!config["redis"])
    {
        std::cerr << "Error : not found redis setting info\n ";        
    }

    redisIP = config["redis"]["ip"].as<std::string>();
    redisPort = config["redis"]["port"].as<int>();
    redisAppName = config["redis"]["app_name"].as<std::string>();

    return;
}
