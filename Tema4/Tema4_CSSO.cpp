#pragma comment(lib, "Wininet")
#include <windows.h>
#include <Wininet.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <string>


using namespace std;

void writeToFile(char* text, char* file)
{
    HANDLE h;
    h = CreateFile(
        file,
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
    DWORD dwBytesToWrite = (DWORD)strlen(text);
    DWORD dwBytesWritten = 0;

    if (!WriteFile(
        h,
        text,
        dwBytesToWrite,
        &dwBytesWritten,
        NULL
    ))
    {
        cout << "Couldn't write to logs.\n";
    }

    CloseHandle(h);

}

DWORD getFileSize(const char* filename)
{
    char filepath[300];
    strcpy(filepath, "C:\\Facultate\\CSSO\\Week4\\Downloads\\");
    strcat(filepath, filename);

    TCHAR Path[300];
    HANDLE hFile;
    DWORD dwRet;

    hFile = CreateFile(
        filepath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        cout << "\nCould not open file: " << GetLastError();
        return 0;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    //char filesizeCh[100];
    //sprintf(filesizeCh, "%d", fileSize);
    CloseHandle(hFile);
    return fileSize;
}

long long int scanDirectory()
{
    WIN32_FIND_DATA fileData;
    //cout << "Accessing directory...\n";
    HANDLE hDir = FindFirstFile("C:\\Facultate\\CSSO\\Week4\\Downloads\\*", &fileData);
    if (hDir == INVALID_HANDLE_VALUE) {
        cout << "\nFindFirstFile error: " << GetLastError() << '\n';
    }
    //else cout << "\nFound directory\n";
    long long int totalSize = 0;

    do {
       //getting the size of the crt file in the given directory
        totalSize += getFileSize(fileData.cFileName);
    } while (FindNextFile(hDir, &fileData) != 0);
    FindClose(hDir);
    return totalSize;
}

HINTERNET getOpenRequestHandle(HINTERNET hConnect, const char* request, const char* address, const char* type)
{
    HINTERNET handleRequest;
    const char* refs[] = { type, NULL };
    cout << "\nRequest: " << request << " " << "Address: " << address << '\n';
    handleRequest = HttpOpenRequest(
        hConnect,
        request,
        address,
        "HTTP/1.1",
        NULL,
        refs,
        0,
        0
    );

    if (handleRequest == NULL)
    {
        cout << "\nError at handleRequest: " << GetLastError();
    }
    return handleRequest;
}

void sendRequest(HINTERNET hRequest, char* toSend)
{
    BOOL sendRequest;
    DWORD dwBytesToWrite;
    if (toSend != NULL) {
        dwBytesToWrite = (DWORD)strlen(toSend);
        cout << "\nSending: " << toSend << endl;
    }
    else dwBytesToWrite = 0;
    
    sendRequest = HttpSendRequest(
        hRequest,
        NULL,
        0,
        toSend,
        dwBytesToWrite
    );

    if (sendRequest == FALSE)
    {
        cout << "\nError at sendRequest: " << GetLastError();
    }
}

char* readFromRequest(HINTERNET hRequest)
{
    BOOL handleRead;
    const DWORD size = 1024;
    DWORD sizeOfRead = 0;
    CHAR buffer[size];
    ZeroMemory(buffer, size);
    handleRead = InternetReadFile(
        hRequest,
        buffer,
        size,
        &sizeOfRead
    );

    if (handleRead == FALSE)
    {
        cout << "\nError at ReadFile: " << GetLastError();
    }
    return (char*)buffer;
}

void scanConfig(HINTERNET hConnect)
{
    string configCommand;
    ifstream file;
    file.open("C:\\Facultate\\CSSO\\Week4\\Downloads\\myconfig.txt");

    if (!file.is_open())
    {
        cout << "\nError at openning file!";
    }
    unsigned int nGet = 0, nPost = 0;
    HINTERNET openRequestHandle;
    while (getline(file, configCommand))
    {
        char cpy[1024];
        strcpy(cpy, configCommand.c_str());
        char* ptr;  
 
        ptr = strtok(cpy, ":");
        char* request = ptr;
        ptr = strtok(NULL, " , ");
        char* address = ptr;
        char* subaddress = address + 27;

        
        char buffer[1024];
        if (!strcmp(request, "GET"))
        {
            nGet++;
            openRequestHandle = getOpenRequestHandle(hConnect, request, subaddress, "text/*");
            sendRequest(openRequestHandle, NULL);
            char* filename = (char*)"C:\\Facultate\\CSSO\\Week4\\Downloads\\";
            char* file = subaddress + 12;
            char cpy[1024];
            strcpy(cpy, filename);
            strcat(cpy, file);
            strcat(cpy, ".txt");
            char content[1024];
            strcpy(content, readFromRequest(openRequestHandle));
            //writing to <path> file
            writeToFile(content, cpy);
            //remembering the last value of a GET request
            strcpy(buffer, content);
        }
        else if (!strcmp(request, "POST"))
        {
            nPost++;
            openRequestHandle = getOpenRequestHandle(hConnect, request, subaddress, "application/x-www-form-urlencoded");
            char toSend[1024];
            strcpy(toSend, "id=310910401RSL201222 & value=");
            strcat(toSend, buffer);
            sendRequest(openRequestHandle, toSend);
        }
    }

    //3
    char log[300];
    sprintf(log, "\nid = 310910401RSL201222 & total = %d & get = %d & post = %d & size = %lld\n",
        nGet + nPost, nGet, nPost, scanDirectory());
    openRequestHandle = getOpenRequestHandle(hConnect, "POST", "/endhomework", "application/x-www-form-urlencoded");
    sendRequest(openRequestHandle, log);
    //cout << "\n\nFinal: " << log;
    
}

char* getConfigFile(HINTERNET hConnect)
{

    HINTERNET reqHandle;
    reqHandle = getOpenRequestHandle(hConnect, "GET", "/assignhomework/310910401RSL201222", "text/*");

    sendRequest(reqHandle, NULL);
    return readFromRequest(reqHandle);
}

int main()
{
    //1
    HINTERNET internetHandle;
    internetHandle = InternetOpen(
        "310910401RSL201222",
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL,
        NULL
    );

    if (internetHandle == NULL)
    {
        cout << "\nError at InternetOpen: " << GetLastError();
    }

    HINTERNET internetConnect;
    internetConnect = InternetConnect(
        internetHandle,
        "cssohw.herokuapp.com",
        INTERNET_DEFAULT_HTTP_PORT,
        NULL,
        NULL,
        INTERNET_SERVICE_HTTP,
        0,
        0
    );

    if (internetConnect == NULL)
    {
        cout << "\nError at InternetConnect: " << GetLastError();
    }

    char config[1024];
    strcpy(config, getConfigFile(internetConnect));
    writeToFile(config, (char*)"C:\\Facultate\\CSSO\\Week4\\Downloads\\myconfig.txt");

    //2
    scanConfig(internetConnect);

}