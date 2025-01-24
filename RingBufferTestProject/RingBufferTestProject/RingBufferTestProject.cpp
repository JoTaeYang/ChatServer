#include <iostream>
#include <wchar.h>
#include <Windows.h>
#include <process.h>
#include "CRingBuffer.h"
char randStr[] = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";

CRingBuffer testR;
int leftCount = sizeof(randStr) - 1;
char* tmp = randStr;
char* tmpDeqBuffer = new char[sizeof(randStr)];
int g_ret_enqVal = 0;

DWORD WINAPI EnqueueProc(LPVOID lpParam)
{
    int randEque;
    if (leftCount == 0)
        return 0;
    randEque = rand() % leftCount + 1;

    int ret_enqVal = testR.Enqueue(tmp, randEque);
    tmp += ret_enqVal;
    g_ret_enqVal = ret_enqVal;
    return 0;
}

DWORD  WINAPI DequeueProc(LPVOID lpParam)
{
    int randDque;
    if (leftCount == 0)
        return 0;
    randDque = rand() % leftCount + 1;
    ZeroMemory(tmpDeqBuffer, sizeof(randStr));
    int ret_deqVal = testR.Dequeue(tmpDeqBuffer, randDque);

    if (ret_deqVal > 0)
        printf("%s", tmpDeqBuffer);

    return 0;
}


int main()
{
    srand(30);

    DWORD dwThreadID[3];
    HANDLE hThread[2];

    int randEque;
    int randDque;

    while (1)
    {
        //if (leftCount == 0)
        //{
        //    tmp = randStr;
        //    leftCount = sizeof(randStr) - 1;
        //}

        //hThread[0] = CreateThread(
        //    NULL, 0,
        //    EnqueueProc,
        //    NULL, 0, &dwThreadID[0]
        //);
        //
        //hThread[1] = CreateThread(
        //    NULL, 0,
        //    DequeueProc,
        //    NULL, 0, &dwThreadID[1]
        //);

        //WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

        //leftCount -= g_ret_enqVal;
        //CloseHandle(hThread[0]);
        //CloseHandle(hThread[1]);
        //Sleep(10);

        if (leftCount == 0)
        {
            tmp = randStr;
            leftCount = sizeof(randStr) - 1;
        }

        randEque = rand() % leftCount + 1;
        int ret_enqVal = testR.Enqueue(tmp, randEque);

        tmp += ret_enqVal;

        randDque = rand() % leftCount + 1;
        ZeroMemory(tmpDeqBuffer, sizeof(randStr));
        int ret_deqVal = testR.Dequeue(tmpDeqBuffer, randDque);

        if (ret_deqVal > 0)
            printf("%s", tmpDeqBuffer);

        leftCount -= ret_enqVal;
        Sleep(10);
    }
}