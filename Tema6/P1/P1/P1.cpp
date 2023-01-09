#include <windows.h>
#include <sysinfoapi.h>
#include <iostream>
#include<string>
#include<cstring>
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

void getHT()
{
    cout << "Hyperthreaded systems...\n";
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    DWORD length = 0;
    bool done = false;
    while (!done) {
        cout << "GetLogicalProcessorInformation attempt...\n";
        if (!GetLogicalProcessorInformation(buffer, &length))
        {
            cout << "GetLogicalProcessorInformation error: " << GetLastError() << '\n';
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer)
                    free(buffer);
                buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
                        length);
            }
        }
        else {
            done = true;

            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
            DWORD processorCoreCount = 0;
            int count = 0;
            DWORD byteOffset = 0;
            ptr = buffer;
            while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= length)
            {
                ++count;
                char content[1024];
                sprintf(content, "Logical processor found %d\n---Relationship:%s\n---ProcessorMask:%s\n",
                    count,
                    to_string(ptr->Relationship).c_str(),
                    to_string(ptr->ProcessorMask).c_str());
                writeToFile(content, (char*)"C:\\Facultate\\CSSO\\Week6\\info.txt");
                cout << content << '\n';
                switch (ptr->Relationship)
                {
                    case RelationProcessorCore:
                        processorCoreCount++;
                        break;
                }
                byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
                ptr++;
            }
            cout << "Number of physical processors: " << processorCoreCount << '\n';
        }
    }
}

void getNUMA()
{
    /*
    cout << "NUMA..." << '\n';
    HANDLE hProcess = GetCurrentProcess();
    ULONG_PTR appAff;
    ULONG_PTR sysAff;
    if (GetProcessAffinityMask(hProcess, &appAff, &sysAff)) {
        cout << "ProcessAffinityMask: " << appAff << '\n';
        cout << "SystemAffinityMask: " << sysAff << '\n';
    }
    else cout << "GetProcessAffinityMask error: " << GetLastError() << '\n';

    ULONG nProcessors;
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    nProcessors = sysInfo.dwNumberOfProcessors;
    for (UCHAR i = 0; i < nProcessors; i++)
    {
        UCHAR nodeNumber;
        if (!GetNumaProcessorNode(i, &nodeNumber))
        {
            cout << "GetNumaProcessorNode error: " << GetLastError() << '\n';
        }

        cout << "CPU " << (ULONG)i << ": node " << nodeNumber << '\n';
    }*/

    ULONG highestNodeNumer;
    if (!GetNumaHighestNodeNumber(&highestNodeNumer))
    {
        cout << "GetHighestNodeNumber error: " << GetLastError() << '\n';
    }
    else {
        cout << "Highest node number: " << highestNodeNumer << '\n';
        char content[1024];
        strcpy(content, "\nHighest node number: ");
        strcat(content, to_string(highestNodeNumer).c_str());
        writeToFile(content, (char*)"C:\\Facultate\\CSSO\\Week6\\info.txt");

    }

    ULONGLONG processorMask = 0;
    if (!GetNumaNodeProcessorMask((UCHAR)highestNodeNumer, &processorMask))
    {
        cout << "GetNumaNodeProcessorMask error: " << GetLastError() << '\n';
    }
    else {
        cout << "NumaNodeProcessorMask: " << processorMask << '\n';
        char content[1024];
        strcpy(content, "\nNumaNodeProcessorMask: ");
        strcat(content, to_string(processorMask).c_str());
        writeToFile(content, (char*)"C:\\Facultate\\CSSO\\Week6\\info.txt");
    }
}

void getCPUSets()
{
    /*
    PULONG cpuSetIds = NULL;
    ULONG cpuSetIdCount = 0;
    ULONG requiredIdCount = 0;
    bool done = false;
    while (!done)
    {
        if (!GetProcessDefaultCpuSets(
            GetCurrentProcess(),
            cpuSetIds,
            cpuSetIdCount,
            &requiredIdCount
        )) {
            cout << "GetProcessDefaultCpuSets error: " << GetLastError() << '\n';
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                cout << "idk\n";
                done = true;
            }
        }
        else {
            done = true;
            for (int i = 0; i < sizeof(cpuSetIds); i++)
                cout << cpuSetIds[i] << '\n';
        }
    }*/

    PSYSTEM_CPU_SET_INFORMATION infoBuf = NULL;
    ULONG returnedLength;
    (void)GetSystemCpuSetInformation(
        nullptr,
        0,
        &returnedLength,
        GetCurrentProcess(),
        0);
    infoBuf = (PSYSTEM_CPU_SET_INFORMATION)malloc(
        returnedLength);
    if (!GetSystemCpuSetInformation(infoBuf, returnedLength,
        &returnedLength,
        GetCurrentProcess(),
        0)) {
        cout << "GetSystemCpuSetInformation error: " << GetLastError() << '\n';
    }
    else {
        size_t count = 0;
        while (returnedLength > 0)
        {
            returnedLength -= infoBuf[count].Size;
            ++count;
        }
        char content[1024];
        cout << "Number of CPU cores: " << count << '\n';
        strcpy(content, "\nNumber of CPU cores: ");
        strcat(content, to_string(count).c_str());
        for (size_t i = 0; i < count; i++) {
            cout << "CPU core ID: " << infoBuf[i].CpuSet.Id << '\n';
            strcat(content, "\nCPU core ID: ");
            strcat(content, to_string(infoBuf[i].CpuSet.Id).c_str());
        }
        writeToFile(content, (char*)"C:\\Facultate\\CSSO\\Week6\\info.txt");
    }

}

int main()
{
    getHT();
    getNUMA();
    getCPUSets();
}

