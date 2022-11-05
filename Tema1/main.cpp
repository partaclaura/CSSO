#include<windows.h>
#include<stdio.h>
#include<string>
#include <fstream>
#include<iostream>
#include<Windows.h>
using namespace std;


bool createHiveFile(string fileName) {
	HANDLE h; //creates handle

	//converts string to LPTSTR to be able to pass as parameter
	LPTSTR long_string = new TCHAR[fileName.size() + 1];
	strcpy(long_string, fileName.c_str());

	//constructing the final string represending the path + file name
	char finalFile[300] = "";
	strcat(finalFile, "C:\\Facultate\\CSSO\\Laboratoare\\Week1\\Rezultate\\");
	strcat(finalFile, long_string);

	//create file using handle
	h = CreateFile( finalFile
		, GENERIC_READ, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE) {
		printf("Error la creare fisier: %d\n", GetLastError());
		return false;
	}
	CloseHandle(h);
	return true;
}

HKEY kType(string fileName) {
	if (fileName == "HKLM.txt")
		return HKEY_LOCAL_MACHINE;
	else if (fileName == "HKCC.txt")
		return HKEY_CURRENT_CONFIG;
	return HKEY_CURRENT_USER;
}

void writeToFile(string fileName, char* DataBuffer) {
	//opening file for writing using handle
	HANDLE h;

	//converts string to LPTSTR to be able to pass as parameter
	LPTSTR long_string = new TCHAR[fileName.size() + 1];
	strcpy(long_string, fileName.c_str());

	//constructing the final string represending the path + file name
	char finalFile[300] = "";
	strcat(finalFile, "C:\\Facultate\\CSSO\\Laboratoare\\Week1\\Rezultate\\");
	strcat(finalFile, long_string);

	h = CreateFile(finalFile,
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
		cout << "Wrote key info to " << fileName << '\n';
	}
	else cout << "Couldn't write key info to " << fileName << '\n';

	CloseHandle(h);
}

void getInfo(string fileName)
{
	HKEY hkey;
	DWORD get_subkeys;
	DWORD get_maxsubkey;
	FILETIME get_lastwr;
	DWORD result;
	//extracting key info 
	if (RegOpenKeyEx(kType(fileName)
		, NULL, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		cout << "Extracted info for " << fileName << '\n';
	}
	else cout << "Couldn't extract info for " << fileName << '\n';

	result = RegQueryInfoKey(hkey, 0, 0, 0, &get_subkeys, &get_maxsubkey,
		0, 0, 0, 0, 0, &get_lastwr);
	SYSTEMTIME stime;
	FileTimeToSystemTime(
		&get_lastwr,
		&stime
	);
	char lw[100] = "";
	sprintf(lw, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d",
		stime.wYear, stime.wMonth, stime.wDay, 
		stime.wHour, stime.wMinute, stime.wSecond);

	char DataBuffer[300] = "";
	char subk[100];
	char maxs[100];
	sprintf(subk, "%d", get_subkeys);
	sprintf(maxs, "%d", get_maxsubkey);
	strcat(DataBuffer, subk);
	strcat(DataBuffer, "\n");
	strcat(DataBuffer, maxs);
	strcat(DataBuffer, "\n");
	strcat(DataBuffer, lw);
	writeToFile(fileName, DataBuffer);

	RegCloseKey(hkey);
}

int main()
{
	//part 1
	
	CreateDirectory("C:\\Facultate", NULL);
	CreateDirectory("C:\\Facultate\\CSSO", NULL);
	CreateDirectory("C:\\Facultate\\CSSO\\Laboratoare", NULL);
	CreateDirectory("C:\\Facultate\\CSSO\\Laboratoare\\Week1", NULL);
	CreateDirectory("C:\\Facultate\\CSSO\\Laboratoare\\Week1\\Extensii", NULL);
	CreateDirectory("C:\\Facultate\\CSSO\\Laboratoare\\Week1\\Rezultate", NULL);

	//part2

	char filesToCreate[3][30];
	strcpy(filesToCreate[0], "HKLM.txt");
	strcpy(filesToCreate[1], "HKCC.txt");
	strcpy(filesToCreate[2], "HKCU.txt");

	for (int i = 0; i < 3; i++) {
		if (createHiveFile(filesToCreate[i]))
			cout << "Created file " << filesToCreate[i] << '\n';
		getInfo(filesToCreate[i]);
	}

	//part 3
	if (createHiveFile("sumar.txt")) {
		cout << "Created file sumar.txt\n";
		char DataBuffer[400] = "";
		for (int i = 0; i < 3; i++) {
			char filepath[300];
			strcpy(filepath, "C:\\Facultate\\CSSO\\Laboratoare\\Week1\\Rezultate\\");
			strcat(filepath, filesToCreate[i]);

			TCHAR Path[300];
			HANDLE hFile;
			DWORD dwRet;

			//opening file corresponding to hive for reading info: path and size
			hFile = CreateFile(filepath,              
				GENERIC_READ,          
				FILE_SHARE_READ,       
				NULL,                  
				OPEN_EXISTING,         
				FILE_ATTRIBUTE_NORMAL, 
				NULL);                 

			if (hFile == INVALID_HANDLE_VALUE)
			{
				printf("Could not open file (error %d\n)", GetLastError());
			}

			dwRet = GetFinalPathNameByHandle(hFile, Path, 300, VOLUME_NAME_NT);
			if (dwRet < 300)
			{
				strcat(DataBuffer, filesToCreate[i]);
				strcat(DataBuffer, ":{file_path: ");
				strcat(DataBuffer, Path);
			}
			else printf("\nThe required buffer size is %d.\n", dwRet);

			DWORD fileSize = GetFileSize(hFile, NULL);
			char filesizeCh[100];
			sprintf(filesizeCh, "%d", fileSize);
			strcat(DataBuffer, ", file_size: ");
			strcat(DataBuffer, filesizeCh);
			strcat(DataBuffer, "}\n");

			CloseHandle(hFile);

			writeToFile("sumar.txt", DataBuffer);
		}
	}

}
