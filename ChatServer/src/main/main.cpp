#include <iostream>
#include <WinSock2.h>
#include "CSession.h"
#include "CSetting.h"


int main()
{    
    const std::string configFileName = "../config/config.yaml";

    CSetting serverSetting(configFileName);
    CSession session = CSession();

    std::cout << serverSetting.GetPort() << "\n";    
}