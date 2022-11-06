#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <tlhelp32.h>
#include <conio.h>
#include <Shlwapi.h>
//#include "Incomes.h"
#include <fstream>
#include <sstream>

using namespace std;

#define BUFFER_SIZE 256

long long int getOldValue(char* file)
{
    char path[300] = "C:\\Facultate\\CSSO\\Week3\\Reports\\Summary\\";
    strcat(path, file);
    ifstream crtFile;
    crtFile.open(path);
    if (!crtFile.is_open())
    {
        cout << "Error when opening file\n";
        return 0;
    }
    string value;
    long int suma = 0;
    getline(crtFile, value);
    suma += stoi(value);
    return suma;
}


void updateWithNewValue(long long int crtSum)
{
    long long int oldSum = getOldValue((char*)"income.txt");
    long long int newSum = oldSum + crtSum;
    HANDLE h;

    h = CreateFile(
        "C:\\Facultate\\CSSO\\Week3\\Reports\\Summary\\income.txt",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE)
    {
        cout << "CreateFile error on summary: " << GetLastError() << '\n';
    }
    char sum[300];
    sprintf(sum, "%lld", newSum);
    DWORD dwBytesToWrite = (DWORD)strlen(sum);
    DWORD dwBytesWritten = 0;

    if (!WriteFile(
        h,
        sum,
        dwBytesToWrite,
        &dwBytesWritten,
        NULL
    ))
    {
        cout << "Couldn't write new sum.\n";
    }
    CloseHandle(h);
}

void updateSummary(long long int crtSum)
{
    long long int oldSum = getOldValue((char*)"summary.txt");
    long long int newSum = oldSum + crtSum;
    HANDLE h;

    h = CreateFile(
        "C:\\Facultate\\CSSO\\Week3\\Reports\\Summary\\summary.txt",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE)
    {
        cout << "CreateFile error on summary: " << GetLastError() << '\n';
    }
    char sum[300];
    sprintf(sum, "%lld", newSum);
    DWORD dwBytesToWrite = (DWORD)strlen(sum);
    DWORD dwBytesWritten = 0;

    if (!WriteFile(
        h,
        sum,
        dwBytesToWrite,
        &dwBytesWritten,
        NULL
    ))
    {
        cout << "Couldn't write new sum.\n";
    }
    CloseHandle(h);
}

void createDailySum(char* day, char* sum)
{
    HANDLE h;
    char path[300] = "C:\\Facultate\\CSSO\\Week3\\Reports\\Daily\\";
    strcat(path, day);
    strcat(path, "_income.txt");

    h = CreateFile(
        path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE)
    {
        cout << "CreateFile error on daily sum: " << GetLastError() << '\n';
    }
    DWORD dwBytesToWrite = (DWORD)strlen(sum);
    DWORD dwBytesWritten = 0;

    if (!WriteFile(
        h,
        sum,
        dwBytesToWrite,
        &dwBytesWritten,
        NULL
    ))
    {
        cout << "Couldn't write daily sum.\n";
    }

    CloseHandle(h);
}

void writeToLogs(char* file, string value)
{
    HANDLE h;
    char DataBuffer[300] = "\nAm facut o incasare de ";
    strcat(DataBuffer, value.c_str());
    strcat(DataBuffer, " in data de ");
    strcat(DataBuffer, file);

    h = CreateFile(
        "C:\\Facultate\\CSSO\\Week3\\Reports\\logs.txt",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE) {
        cout << "CreateFile error: " << GetLastError() << '\n';
    }
    SetFilePointer(h, 0, 0, FILE_END);
    DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
    DWORD dwBytesWritten = 0;

    if (!WriteFile(
        h,
        DataBuffer,
        dwBytesToWrite,
        &dwBytesWritten,
        NULL
    ))
    {
        cout << "Couldn't write to logs.\n";
    }

    CloseHandle(h);

}

long int handleFile(char* file)
{
    char path[300] = "C:\\Users\\Laura\\Desktop\\Tema3_CSSO\\income\\";
    strcat(path, file);
    //cout << "Full path: " << path << '\n';
    ifstream crtFile;
    crtFile.open(path);
    if (!crtFile.is_open())
    {
        cout << "Error when opening file\n";
    }
    string value;
    long int suma = 0;
    while (getline(crtFile, value))
    {
        writeToLogs(file, value);
        suma += stoi(value);
    }
    char sum[300];
    sprintf(sum, "%d", suma);
    createDailySum(file, sum);
    return suma;
}

long long int scanDirectory()
{
    WIN32_FIND_DATA fileData;
    cout << "Accessing directory...\n";
    HANDLE hDir = FindFirstFile("C:\\Users\\Laura\\Desktop\\Tema3_CSSO\\income\\*", &fileData);
    if (hDir == INVALID_HANDLE_VALUE) {
        cout << "FindFirstFileA error: " << GetLastError() << '\n';
    }
    else cout << "Found directory\n";
    long long int totalSum = 0;
    do {
        //cout << fileData.cFileName << '\n';
        totalSum += handleFile(fileData.cFileName);
    } while (FindNextFile(hDir, &fileData) != 0);
    updateWithNewValue(totalSum);
    FindClose(hDir);
    return totalSum;
}

char* readFromMemoryMap() {
    TCHAR sz_name[] = TEXT("cssohw3management");

    HANDLE mapHandle;
    LPCTSTR buf;

    mapHandle = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        sz_name);

    if (mapHandle == NULL) {
        cout << "OpenFileMapping error: " << GetLastError() << '\n';
    }

    buf = (LPTSTR)MapViewOfFile(
        mapHandle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        BUFFER_SIZE);

    if (buf == NULL) {
        cout << "MapViewOfFile error: " << GetLastError() << '\n';
    }

    cout << "Read from memory mapping: " << '\n' << (char*)buf << '\n';
    return (char*)buf;

    UnmapViewOfFile(buf);
    CloseHandle(mapHandle);
}

void writeToMemoryMap(char* message) {
    TCHAR sz_name[] = TEXT("cssohw3management");
    TCHAR sz_message[1024] = TEXT("");
    _tcscpy(sz_message, (TCHAR*)message);
    //cout << "sz_msg: " << (char*)sz_message << '\n';
    HANDLE mapHandle;
    LPCTSTR buf;

    mapHandle = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        sz_name);

    if (mapHandle == NULL) {
        cout << "OpenFileMapping error: " << GetLastError();
    }
    else cout << "OpenFileMapping ok \n";

    buf = (LPTSTR)MapViewOfFile(
        mapHandle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        BUFFER_SIZE);

    if (buf == NULL) {
        cout << "MapViewOfFile error: " << GetLastError();
    }
    else cout << "MapViewOfFile ok \n";
    CopyMemory((PVOID)buf, sz_message, (_tcslen(sz_message) * sizeof(TCHAR)));
    cout << "Wrote to map \n";

    UnmapViewOfFile(buf);
    CloseHandle(mapHandle);
}

int main()
{
    cout << "Income.exe started.\n";
    HANDLE hMutex1 = OpenMutex(
        SYNCHRONIZE,
        FALSE,
        "logsMutex"
    );

    if (hMutex1 == NULL)
    {
        cout << "OpenMutex error: " << GetLastError() << '\n';
    }
    WaitForSingleObject(hMutex1, INFINITE);
    long long int sum = scanDirectory();
    ReleaseMutex(hMutex1);
    CloseHandle(hMutex1);

    HANDLE hMutex2 = OpenMutex(
        SYNCHRONIZE,
        FALSE,
        "summaryMutex"
    );

    if (hMutex2 == NULL)
    {
        cout << "OpenMutrx error: " << GetLastError() << '\n';
    }
    WaitForSingleObject(hMutex2, INFINITE);
    updateSummary(sum);
    ReleaseMutex(hMutex2);
    CloseHandle(hMutex2);

    HANDLE eventHandle;
    eventHandle = OpenEvent(
        EVENT_MODIFY_STATE,
        FALSE,
        "memoryMapEvent"
    );
    WaitForSingleObject(eventHandle, INFINITE);
    long long int oldSum = atoi(readFromMemoryMap());
    long long int newSum = oldSum - sum;
    char newSumM[300];
    sprintf(newSumM, "%lld", newSum);
    writeToMemoryMap(newSumM);
    SetEvent(eventHandle);
    CloseHandle(eventHandle);
}