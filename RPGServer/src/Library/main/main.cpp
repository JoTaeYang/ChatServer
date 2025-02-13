﻿#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <process.h>
#include <atomic>
#include <mutex>
#include <optional>


#include "Library/Session/CSession.h"
#include "Library/Setting/CSetting.h"
#include "RPGServer/Server/RPGServer.h"
#include "Library/Server/CServer.h"


int main()
{    
    const std::string configFileName = "../config/config.yaml";

    CSetting serverSetting(configFileName);
    RPGServer server;

    server.Start(1000, serverSetting);


    while (1)
    {
        Sleep(1000);
    }
    
}