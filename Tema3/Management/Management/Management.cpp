#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <tlhelp32.h>
#include <conio.h>
#include <fstream>

using namespace std;

#define BUFFER_SIZE 256
TCHAR map_name[] = TEXT("cssohw3management");

void createDirectories()
{
    CreateDirectoryA("C:\\Facultate\\CSSO\\Week3\\Reports", NULL);
    CreateDirectoryA("C:\\Facultate\\CSSO\\Week3\\Reports\\Daily", NULL);
    CreateDirectoryA("C:\\Facultate\\CSSO\\Week3\\Reports\\Summary", NULL);
}

long long int getValueFromFile(char* file)
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
    getline(crtFile, value);
    return  stoi(value);
}

void createMemoryMap()
{
    HANDLE mapHandle;
    LPCSTR buffer;

    cout << "Creating file mapping...\n";
    mapHandle = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        BUFFER_SIZE,
        map_name
    );

    if (mapHandle == NULL) {
        cout << "CreateFileMapping error: " << GetLastError() << '\n';
    }
    else cout << "Created file mapping successfully\n";
    
    cout << "Creating map view of file...\n";
    buffer = (LPCSTR)MapViewOfFile(
        mapHandle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        BUFFER_SIZE
    );
    cout << "Got buffer\n";
    TCHAR sz_message[1024] = TEXT("0");
    if (buffer == NULL) {
        cout << "MapViewOfFile error: " << GetLastError();
    }
    else {
        cout << "Writing to memory...\n";
        if (CopyMemory((PVOID)buffer, sz_message, (_tcslen(sz_message) * sizeof(TCHAR))) == NULL)
            cout << "CopyMemory error: " << GetLastError();
        cout << "Wrote to map successfully\n";
    }
    
}

void readFromMemoryMap()
{
    HANDLE mapHandle;
    LPCTSTR buffer;

    mapHandle = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        map_name
    );

    if (mapHandle == NULL) {
        cout << "OpenFileMapping error: " << GetLastError();
    }
    else cout << "Opened File Mapping successfully\n";

    buffer = (LPTSTR)MapViewOfFile(
        mapHandle,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        BUFFER_SIZE
    );

    if (buffer == NULL) {
        cout << "MapViewOfFile error: " << GetLastError();
    }
    else
        cout << "Read from memory mapping: " << (char*)buffer << '\n';

    UnmapViewOfFile(buffer);
    CloseHandle(mapHandle);
}

void launchProcs() 
{

    string procs[2] = {
        "C:\\Users\\Laura\\Desktop\\Tema3_CSSO\\Pay\\x64\\Debug\\Pay.exe",
        "C:\\Users\\Laura\\Desktop\\Tema3_CSSO\\Incomes\\x64\\Debug\\Incomes.exe"
    };

    for (int i = 0; i < 2; i++)
    {
        cout << "Launching process " << procs[i] << "...\n";
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        LPTSTR long_string = new TCHAR[procs[i].size() + 1];
        strcpy(long_string, procs[i].c_str());
        if (!CreateProcess(long_string,
            NULL,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)) {
            cout << "CreateProcess error: " << GetLastError() << '\n';
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        cout << "Process " << procs[i] << " done\n";
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }


    
}

void launchGenerate()
{
    cout << "Launching process Generate.exe" << "...\n";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    string proc = "C:\\Users\\Laura\\Desktop\\Tema3_CSSO\\Generate\\x64\\Debug\\Generate.exe";
    LPTSTR long_string = new TCHAR[proc.size() + 1];
    strcpy(long_string, proc.c_str());
    if (!CreateProcess(long_string,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi)) {
        cout << "CreateProcess error: " << GetLastError() << '\n';
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    cout << "Process " << proc << " done\n";
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main()
{
    HANDLE handleEvent;
    handleEvent = CreateEvent(
        NULL,
        FALSE,
        FALSE,
        "memoryMapEvent"
    );
    HANDLE handleMutex1, handleMutex2;
    handleMutex1 = CreateMutex(
        NULL,
        FALSE,
        "logsMutex"
    );

    handleMutex2 = CreateMutex(
        NULL,
        FALSE,
        "summaryMutex"
    );

    if (handleMutex1 == NULL || handleMutex2 == NULL)
    {
        cout << "CreateMutex error: " << GetLastError() << '\n';
    }
    /*
     creaza directorul "C : \Facultate\CSSO\Week3\Reports"
     creaza directorul "C:\Facultate\CSSO\Week3\Reports\Daily"
     creaza directorul "C:\Facultate\CSSO\Week3\Reports\Summary"
     */
    //createDirectories();
    
    //Creaza fisierul mapat in memorie "cssohw3management" in care scrie
    //valoarea 0
    createMemoryMap();
    
    //Lanseaza simultan in executie "pay.exe", "income.exe" si asteapta
    //terminarea lor
    launchProcs();
    
    //Lanseaza in executie "generate.exe" si asteapta terminarea lui
    launchGenerate();

    /* Afiseaza valorile numerice din
        "C : \Facultate\CSSO\Week3\Reports\Summary\income.txt",
        "C:\Facultate\CSSO\Week3\Reports\Summary\payments.txt",
        "C:\Facultate\CSSO\Week3\Reports\Summary\summary.txt"
    */
    cout << "Income: " << getValueFromFile((char*)"income.txt") << '\n';
    cout << "Payments: " << getValueFromFile((char*)"payments.txt") << '\n';
    cout << "Summary: " << getValueFromFile((char*)"summary.txt") << '\n';

    //Citește valoarea din “cssohw3management” și o afișează pe ecran
    readFromMemoryMap();

    CloseHandle(handleMutex1);
    CloseHandle(handleMutex2);
    CloseHandle(handleEvent);
}


