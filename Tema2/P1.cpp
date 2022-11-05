
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <tlhelp32.h>
#include <conio.h>
#include "atlstr.h"

using namespace std;
#define BUFFER_SIZE 1024


void setPrivilege() {
    HANDLE hProcess = GetCurrentProcess();
    HANDLE tokenHandle;
    if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &tokenHandle)) {
        TOKEN_PRIVILEGES tp;
        LUID luid;
        if (!LookupPrivilegeValueW(NULL, SE_SYSTEM_PROFILE_NAME, &luid)) {
            cout << "LookupPrivilegeValueW error: " << GetLastError();
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = 0;

        if (!AdjustTokenPrivileges(
            tokenHandle,
            FALSE,
            &tp,
            sizeof(TOKEN_PRIVILEGES),
            (PTOKEN_PRIVILEGES)NULL,
            (PDWORD)NULL
        )) {
            cout << "AdjustTokenPrivileges error: " << GetLastError();
        }
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
            cout << "The token doesn't have the specified privilege! :(";
        }
        else cout << "The token has the specified privilege! :)";

    }
    else {
        cout << "OpenProcessToken error: " << GetLastError();
    }
}

void createDir() {
    CreateDirectoryA("C:\\Facultate", NULL);
    CreateDirectoryA("C:\\Facultate\\CSSO", NULL);
    CreateDirectoryA("C:\\Facultate\\CSSO\\Laboratoare", NULL);
    CreateDirectoryA("C:\\Facultate\\CSSO\\Laboratoare\\Week2", NULL);
}

bool createFile(string fileName) {
    HANDLE h; //creates handle

    //constructing the final string represending the path + file name
    char finalFile[300] = "";
    strcat(finalFile, "C:\\Facultate\\CSSO\\Laboratoare\\Week2\\");
    strcat(finalFile, fileName.c_str());

    //create file using handle
    h = CreateFileA(finalFile
        , GENERIC_READ, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        printf("Error la creare fisier: %d\n", GetLastError());
        return false;
    }
    CloseHandle(h);
    return true;
}

void writeToFile(string fileName, char* DataBuffer) {
    //opening file for writing using handle
    HANDLE h;

    //constructing the final string represending the path + file name
    char finalFile[300] = "";
    strcat(finalFile, "C:\\Facultate\\CSSO\\Laboratoare\\Week2\\");
    strcat(finalFile, fileName.c_str());

    h = CreateFileA(finalFile,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (h == INVALID_HANDLE_VALUE) {
        printf("Error la creare fisier: %d\n", GetLastError());
    }

    DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
    DWORD dwBytesWritten = 0;

    if (WriteFile(
        h,
        DataBuffer,
        dwBytesToWrite,
        &dwBytesWritten,
        NULL)) {
        cout << "Wrote info to " << fileName << '\n';
    }
    else cout << "Couldn't write info to " << fileName << '\n';

    CloseHandle(h);
}

int ThreadInfo() {
    //HANDLE hProcess;
    THREADENTRY32 te32;

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        cout << "CreateToolhelp32Snapshot error: " << GetLastError();
    }

    te32.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(hProcessSnap, &te32)) {
        cout << "Process32First error: " << GetLastError();
        CloseHandle(hProcessSnap);
    }

    char DataBuffer[400000] = "";
    int count = 0;
    do {
        char crtData[30000] = "";
        sprintf(crtData,
            "Thread: %d, OwnerProcessId: %d \n", 
            te32.th32ThreadID, te32.th32OwnerProcessID);
        strcat(DataBuffer, crtData);
        count++;
    } while (Thread32Next(hProcessSnap, &te32));
    writeToFile("fire.txt", DataBuffer);
    CloseHandle(hProcessSnap);
    return count;
}

int ProcessInfo() {
    PROCESSENTRY32 pe32;

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        cout << "CreateToolhelp32Snapshot error: " << GetLastError();
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        cout << "Process32First error: " << GetLastError();
        CloseHandle(hProcessSnap);
    }

    char DataBuffer[40000] = "";
    int count = 0;
    do {
        char crtData[3000] = "";
        sprintf(crtData,
            "Parent: %d, Process: %d, SzExeFile: %ws \n",
            pe32.th32ParentProcessID, pe32.th32ProcessID, pe32.szExeFile);
        strcat(DataBuffer, crtData);
        count++;
    } while (Process32Next(hProcessSnap, &pe32));
    writeToFile("procese.txt", DataBuffer);
    CloseHandle(hProcessSnap);
    return count;
}

int ModuleInfo() {
    MODULEENTRY32 me32;

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        cout << "CreateToolhelp32Snapshot error: " << GetLastError();
    }

    me32.dwSize = sizeof(MODULEENTRY32);
    if (!Module32First(hProcessSnap, &me32)) {
        cout << "Module32First error: " << GetLastError();
        CloseHandle(hProcessSnap);
    }

    char DataBuffer[40000] = "";
    int count = 0;
    do {
        char crtData[3000] = "";
        sprintf(crtData,
            "Module: %d, Process: %d, SzModule: %ws, SzExeFile: %ws \n",
            me32.th32ModuleID, me32.th32ProcessID, me32.szModule, me32.szExePath);
        strcat(DataBuffer, crtData);
        count++;
    } while (Module32Next(hProcessSnap, &me32));
    writeToFile("module_process.txt", DataBuffer);
    CloseHandle(hProcessSnap);
    return count;
}

void writeToMemoryMap(char* message) {
    TCHAR sz_name[] = TEXT("cssow2basicsync");
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
    //1
    //a
    setPrivilege();

    //b
    createDir();

    //c
    createFile("procese.txt");
    int pcount = ProcessInfo();

    //d
    createFile("fire.txt");
    int tcount = ThreadInfo();

    //e
    createFile("module_process.txt");
    int mcount = ModuleInfo();

    char message[1024];
    sprintf(message, "Module: %d \n Procese: %d \n Fire: %d ",
        mcount, pcount, tcount);
    writeToMemoryMap(message);
    return 0;
}
