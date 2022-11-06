#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <tlhelp32.h>
#include <conio.h>
#include <Shlwapi.h>
#include <fstream>

using namespace std;

#define BUFFER_SIZE 256
TCHAR sz_name[] = TEXT("cssohw3management");

long long int getValueFromFile(char * file)
{
    char path[300] = "C:\\Facultate\\CSSO\\Week3\\Reports\\Daily\\";
    strcat(path, file);
    ifstream crtFile;
    crtFile.open(path);
    if (!crtFile.is_open())
    {
        cout << "Error when opening file\n";
        return 0;
    }
    string value;
    getline(crtFile, value);
    return  stoi(value);
}

void writeToLogs(char* log)
{
    HANDLE h;
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
    DWORD dwBytesToWrite = (DWORD)strlen(log);
    DWORD dwBytesWritten = 0;

    if (!WriteFile(
        h,
        log,
        dwBytesToWrite,
        &dwBytesWritten,
        NULL
    ))
    {
        cout << "Couldn't write to logs.\n";
    }

    CloseHandle(h);

}

long long int scanDirectory()
{
    WIN32_FIND_DATA fileData;
    cout << "Accessing directory...\n";
    HANDLE hDir = FindFirstFile("C:\\Facultate\\CSSO\\Week3\\Reports\\Daily\\*", &fileData);
    if (hDir == INVALID_HANDLE_VALUE) {
        cout << "FindFirstFileA error: " << GetLastError() << '\n';
    }
    else cout << "Found directory\n";
    long long int totalIncome = 0, totalPay = 0;

    do {
        //cout << fileData.cFileName << '\n';
        if (strstr(fileData.cFileName, "income"))
            totalIncome += getValueFromFile(fileData.cFileName);
        else if (strstr(fileData.cFileName, "payment"))
            totalPay += getValueFromFile(fileData.cFileName);
    } while (FindNextFile(hDir, &fileData) != 0);
    FindClose(hDir);

    long long int profit = totalIncome - totalPay;
    char log[300];
    sprintf(log, "\nS-au facut incasari de %lld\nS-au facut plati de %lld\nS-a realizat un profit de %lld\n", 
        totalIncome, totalPay, profit);
    writeToLogs(log);
    return profit;
}

char* readFromMemoryMap()
{
    
    HANDLE mapHandle;
    LPCTSTR buf;

    mapHandle = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        sz_name);

    if (mapHandle == NULL) {
        cout << "OpenFileMapping error: " << GetLastError();
    }

    buf = (LPTSTR)MapViewOfFile(
        mapHandle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        BUFFER_SIZE);

    if (buf == NULL) {
        cout << "MapViewOfFile error: " << GetLastError();
    }

    cout << "Read from memory mapping: " << '\n' << (char*)buf << '\n';
    
    return (char*)buf;
}

int main()
{
    cout << "Generate.exe started.\n";
    HANDLE hMutex = OpenMutex(
        SYNCHRONIZE,
        FALSE,
        "logsMutex"
    );

    if (hMutex == NULL)
    {
        cout << "OpenMutex error: " << GetLastError() << '\n';
    }
    WaitForSingleObject(hMutex, INFINITE);
    long long int generatedProfit = scanDirectory();
    cout << "Generated profit: " << generatedProfit << '\n';
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    HANDLE eventHandle;
    eventHandle = OpenEvent(
        EVENT_MODIFY_STATE,
        FALSE,
        "memoryMapEvent"
    );
    WaitForSingleObject(eventHandle, INFINITE);
    char* managementProfitC = readFromMemoryMap();
    SetEvent(eventHandle);
    CloseHandle(eventHandle);
    long long int managementProfit = atoi(managementProfitC);
    char log[300];
    cout << "Management profit: " << managementProfit << '\n';



    if (generatedProfit == managementProfit)
        strcpy(log, "Raport generat cu success!");
    else strcpy(log, "Ai o greseala in generarea raportului");


    HANDLE hMutex2 = OpenMutex(
        SYNCHRONIZE,
        FALSE,
        "logsMutex"
    );

    if (hMutex2 == NULL)
    {
        cout << "OpenMutex error: " << GetLastError() << '\n';
    }
    WaitForSingleObject(hMutex2, INFINITE);
    writeToLogs(log);
    ReleaseMutex(hMutex2);
    CloseHandle(hMutex2);
    
}

