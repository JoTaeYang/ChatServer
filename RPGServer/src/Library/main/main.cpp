#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <process.h>
#include <atomic>
#include <mutex>
#include <optional>
#include <conio.h>


#include "Library/Session/CSession.h"
#include "Library/Setting/CSetting.h"
#include "RPGServer/Server/RPGServer.h"
#include "Library/Server/CServer.h"
#include "Library/Profile/Profile.h"
#include "Library/MessageBuffer/MessageBuffer.h"


const float X_MIN = -180.0f;
const float X_MAX = 180.0;

float DequantizeInt16ToFloat(short quantizedValue) {
    return (quantizedValue - SHRT_MIN) * (X_MAX - X_MIN) / (SHRT_MAX - SHRT_MIN) + X_MIN;
}


int main()
{
    Profile& profile = Profile::GetInstance();
    
    //float px, py, pz;
    //float rx, ry, rz;
    //float vx, vy, vz;


    //for (int i = 0; i < 10000000; ++i)
    //{
    //    CMessageBuffer* buffer = CMessageBuffer::Alloc();

    //    profile.Begin("Before");
    //    *buffer << 1000.0f << 1000.0f << 1000.0f
    //        << 1000.0f << 1000.0f << 1000.0f
    //        << 1000.0f << 1000.0f << 1000.0f;

    //    *buffer >> px >> py >> pz >> rx >> ry >> rz >> vx >> vy >> vz;
    //    profile.End("Before");
    //    buffer->DecRef();
    //}
    //
    //short srx, sry, srz;
    //short svx, svy, svz;
    //        
    //for (int i = 0; i < 10000000; ++i)
    //{
    //    CMessageBuffer* buffer = CMessageBuffer::Alloc();
    //    short tmpShort = 20000;
    //    profile.Begin("After");
    //    
    //    *buffer << 1000.0f << 1000.0f << 1000.0f
    //        << tmpShort  << tmpShort << tmpShort
    //        << tmpShort << tmpShort << tmpShort;

    //    *buffer >> px >> py >> pz >> srx >> sry >> srz >> svx >> svy >> svz;

    //    rx = DequantizeInt16ToFloat(srx);
    //    ry = DequantizeInt16ToFloat(sry);
    //    rz = DequantizeInt16ToFloat(srz);

    //    vx = DequantizeInt16ToFloat(svx);
    //    vy = DequantizeInt16ToFloat(svy);
    //    vz = DequantizeInt16ToFloat(svz);
    //    profile.End("After");
    //    buffer->DecRef();
    //}

    //

    const std::string configFileName = "../config/config.yaml";

    CSetting serverSetting(configFileName);
    RPGServer server;

    server.Start(1000, serverSetting);


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