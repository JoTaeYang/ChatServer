#include "Library/Profile/Profile.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>


Profile& Profile::GetInstance()
{
    static Profile instance;    
    return instance;
}

void Profile::Begin(const std::string& name)
{
	int threadIdx = IsCheckThread(GetCurrentThreadId());

	int idx = IsCheckName(threadIdx, name);

    if (idx == -1)
    {
        AddProfile(threadIdx, name);
        return;
    }
    else
    {
        StartProfile(threadIdx, idx);
    }

    return;
}

void Profile::End(const std::string& name)
{
    LARGE_INTEGER End;
    LARGE_INTEGER Freq;
    LONGLONG timeDiff;
    double microFreq = 0.0;
    double usingTime;
    int index;
    int tIndex;

    if (!QueryPerformanceFrequency(&Freq))
        return;

    QueryPerformanceCounter(&End);

    tIndex = IsCheckThread(GetCurrentThreadId());
    index = IsCheckName(tIndex, name); // 체크하는 이름의 인덱스 가져오기

    timeDiff = End.QuadPart - profile[tIndex].samples[index].lStartTime.QuadPart;//함수 사용시간

    usingTime = (timeDiff) / (Freq.QuadPart / 1000000.0); //함수 사용시간, 마이크로 세컨드 100만단위로 구함

    profile[tIndex].samples[index].iTotalTime += usingTime; //총 사용시간 집계를 위해 더하기
    profile[tIndex].samples[index].allTime.push_back(usingTime);
    CheckMinMax(tIndex, index, usingTime); //Min과 Max
}

double getPercentile(std::vector<double>& data, double percentile) {
    if (data.empty()) return 0.0;

    std::sort(data.begin(), data.end()); // 정렬
    size_t index = static_cast<size_t>(percentile / 100.0 * data.size());
    if (index >= data.size()) index = data.size() - 1;

    return data[index]; // 해당 백분위 위치 값 반환
}


void Profile::ProfileDataOutText(const std::string& fileName)
{
    std::filesystem::path dir = "/Profile";
    std::filesystem::path filePath = std::filesystem::path(fileName);

    std::filesystem::path fullPath = dir / filePath;

    std::wofstream outFile(fileName);
    outFile.imbue(std::locale("en_US.UTF-8"));

    outFile << "Id  |  Name  |     Average      |      Min      |      Max      |      90      |      99      |     Call   |\n";    

    for (int tiCnt = 0; tiCnt < MAX_PROFILE; tiCnt++)
    {
        outFile << "-----------------------------------------------\n";        
        for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
        {
            if (profile[tiCnt].samples[iCnt].lFlag == 1)
            {
                std::wostringstream woss;
  
                woss << std::setw(2) << profile[tiCnt].id << L"  |  "
                    << std::setw(10) << profile[tiCnt].samples[iCnt].name.c_str() << L"    |     "
                    << std::fixed << std::setprecision(4)
                    << std::setw(10) << (profile[tiCnt].samples[iCnt].iTotalTime / profile[tiCnt].samples[iCnt].iCall) / 1.0 << L" ㎲ | "
                    << std::setw(10) << profile[tiCnt].samples[iCnt].iMin[0] / 1.0 << L" ㎲ |  "
                    << std::setw(10) << profile[tiCnt].samples[iCnt].iMax[0] / 1.0 << L" ㎲ |  "
                    << std::setw(10) << getPercentile(profile[tiCnt].samples[iCnt].allTime, 90) / 1.0 << L" ㎲ |  "
                    << std::setw(10) << getPercentile(profile[tiCnt].samples[iCnt].allTime, 99) / 1.0<< L" ㎲ |  "
                    << profile[tiCnt].samples[iCnt].iCall << L"\n";
                outFile << woss.str();
            }
        }        
    }

    outFile.close();    
}

void Profile::CheckMinMax(int id, int index, double time)
{
    double tmp = 0;
    if (profile[id].samples[index].iMin[0] == 0)
    {
        profile[id].samples[index].iMin[0] = time;
        return;
    }
    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (profile[id].samples[index].iMin[iCnt] > time)
        {
            tmp = time;
            time = profile[id].samples[index].iMin[iCnt];
            profile[id].samples[index].iMin[iCnt] = tmp;
        }
    }

    for (int iCnt = 0; iCnt < 2; iCnt++)
    {
        if (profile[id].samples[index].iMax[iCnt] < time)
        {
            tmp = time;
            time = profile[id].samples[index].iMax[iCnt];
            profile[id].samples[index].iMax[iCnt] = tmp;
        }
    }
}

int Profile::IsCheckName(int index, const std::string& name)
{
    for (int iCnt = 0; iCnt < MAX_SAMPLE; iCnt++)
    {
        if (profile[index].samples[iCnt].lFlag == 1)
        {            
            if (profile[index].samples[iCnt].name.compare(name) == 0)
            {
                return iCnt;
            }            
        }
    }
	return -1;
}

int Profile::IsCheckThread(int id)
{
	return 0;
}

void Profile::AddProfile(int index, const std::string& name)
{
    for (int i = 0; i < MAX_SAMPLE; ++i)
    {
        if (profile[index].samples[i].lFlag == 0)
        {
            profile[index].samples[i].lFlag = 1;
            profile[index].samples[i].name = std::string(name);
            
            QueryPerformanceCounter(&profile[index].samples[i].lStartTime);

            profile[index].samples[i].iTotalTime = 0;
            profile[index].samples[i].iCall = 1;
            memset(profile[index].samples[i].iMin, 0, sizeof(profile[index].samples[i].iMin) / sizeof(_int64));
            memset(profile[index].samples[i].iMax, 0, sizeof(profile[index].samples[i].iMax) / sizeof(_int64));
            break;
        }
    }
}

void Profile::StartProfile(int id, int index)
{
    QueryPerformanceCounter(&profile[id].samples[index].lStartTime);
    profile[id].samples[index].iCall += 1;
}
