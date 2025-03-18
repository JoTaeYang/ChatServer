#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <process.h>
#include <atomic>
#include <mutex>
#include <optional>
#include <conio.h>

#include <sw/redis++/redis.h>
#include <sw/redis++/redis_cluster.h>


#include "Library/Session/CSession.h"
#include "Library/Setting/CSetting.h"
#include "RPGServer/Server/RPGServer.h"
#include "Library/Server/CServer.h"
#include "Library/Profile/Profile.h"
#include "Library/MessageBuffer/MessageBuffer.h"
#include "Library/DB/CRedisDB.h"



const float X_MIN = -180.0f;
const float X_MAX = 180.0;


int main()
{

    Profile& profile = Profile::GetInstance();
    const std::string configFileName = "../config/config.yaml";

    CSetting serverSetting;

    serverSetting.LoadSettings(configFileName);

    CRedisDB redis(&serverSetting);
    redis.Connect();
    

    RPGServer server;
    server.Start(1000, serverSetting, &redis);

    char str[64];
    while (1)
    {
        Sleep(1000);

        if (_kbhit())
        {
            if (std::cin.peek() == EOF)
            {
                std::cout << "\nCTRL + Z 입력으로 종료합니다.\n";
                std::cin.clear();
                break;
            }
                
        }
    }
    server.Stop();

    profile.ProfileDataOutText("example.txt");
}