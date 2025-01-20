#include <iostream>
#include "CSetting.h"

int main()
{    
    const std::string configFileName = "../config/config.yaml";

    CSetting serverSetting(configFileName);

    std::cout << serverSetting.GetPort() << "\n";    
}