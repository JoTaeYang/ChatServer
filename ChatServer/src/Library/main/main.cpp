#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <process.h>
#include <atomic>
#include <mutex>
#include <optional>

#include "Interfaces/ILock.h"

#include "SpinLock.h"
#include "CLockStack.h"
#include "CSession.h"
#include "CSetting.h"
#include "CServer.h"


int main()
{    
    const std::string configFileName = "../config/config.yaml";

    CSetting serverSetting(configFileName);
    CServer server;

    server.Start(1000, serverSetting);


    while (1)
    {
        Sleep(1000);
    }
    
}