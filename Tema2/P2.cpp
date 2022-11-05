#include <windows.h>
#include <iostream>
#include <tchar.h>
using namespace std;

DWORD WINAPI readFile(LPVOID lpParam) {
    cout << (char*)lpParam << '\n';
    FILE* file = fopen((char*)lpParam, "r");
    char c;
    int newLineCount = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n')
            newLineCount++;
    }
    cout << "New line count for " << (char*)lpParam << " = " << newLineCount << '\n';
    return 0;
}

int main()
{
    DWORD threads[3];
    HANDLE threadHandles[3];
    char paths[3][1024] = { "C:\\Facultate\\CSSO\\Laboratoare\\Week2\\procese.txt" , 
    "C:\\Facultate\\CSSO\\Laboratoare\\Week2\\fire.txt" , "C:\\Facultate\\CSSO\\Laboratoare\\Week2\\module_process.txt" };
    for (int i = 0; i < 3; i++) {

        threadHandles[i] = CreateThread(
            NULL,
            0,
            readFile,
            paths[i],
            0,
            &threads[i]
        );

        if (threads[i] == NULL)
        {
            cout << "CreateThread error: " << GetLastError();
        }
    }
    WaitForMultipleObjects(3, threadHandles, TRUE, INFINITE);
    for (int i = 0; i < 3; i++)
    {
        CloseHandle(threadHandles[i]);
    }
}

