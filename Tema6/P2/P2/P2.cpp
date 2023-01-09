#include <cstdlib>
#include <windows.h>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cstring>
#include<string>
using namespace std;
using namespace std::chrono;

struct environment{
    int** matrix_a;
    int** matrix_b;
    int** matrix_c;
    int kValue;
    int matrixIndex;
    int i;
    int j;
    int worker;
};

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
        cout << "Couldn't write to file.\n";
    }

    CloseHandle(h);

}

void sequentialMul(int** a, int** b, int size, int k, int index)
{
    int** c = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++)
        c[i] = (int*)malloc(size * sizeof(int));
    high_resolution_clock::time_point before = high_resolution_clock::now();
    for (int i = 0; i < size * size; i++)
    {
        int row = i / size;
        int col = i % size;
        c[row][col] = 0;
        for (int k = 0; k < size; k++)
            c[row][col] += a[row][k] * b[k][col];
    }
    high_resolution_clock::time_point after = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(after - before);

    char fileName[256];
    sprintf(fileName, "C:\\Facultate\\CSSO\\Week6\\rezultate\\secvential\\Seq_%d_%d_%s.txt",
        k, index, std::to_string(time_span.count()).c_str());
    for (int i = 0; i < size * size; i++)
    {
        char buffer[256];
        sprintf(buffer, "%d\n", c[i / size][i % size]);
        writeToFile(buffer, fileName);
    }

}

DWORD WINAPI multiplicateMatrix(LPVOID lpParam)
{
    struct environment *env = (struct environment*)lpParam;
    int size = pow(10, env->kValue);
    int* c = (int*)malloc((env->j - env->i) * sizeof(int));
    int cindex = 0;
    high_resolution_clock::time_point before = high_resolution_clock::now();
    //cout << "Elements from " << env->i << " to " << env->j << "\n";
    for (int i = env->i; i < env->j; i++)
    {
        int row = i / size;
        int col = i % size;
        env->matrix_c[row][col] = 0;
        for (int k = 0; k < size; k++) {
            env->matrix_c[row][col] += env->matrix_a[row][k] * env->matrix_b[k][col]; 
        }
        c[cindex++] = env->matrix_c[row][col];
    }
    high_resolution_clock::time_point after = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(after - before);
    char fileName[256];
    sprintf(fileName, "C:\\Facultate\\CSSO\\Week6\\rezultate\\static\\%d_%d_%d_%s.txt",
        env->worker, env->kValue, env->matrixIndex, std::to_string(time_span.count()).c_str());
    cindex--;
    for (int i = 0; i < cindex; i++)
    {
        char buffer[256];
        sprintf(buffer, "%d\n", c[(i / cindex) * cindex + i % cindex]);
        writeToFile(buffer, fileName);
    }
    return 0;
}

void staticMul(int** a, int** b, int k, int index)
{
    //procesoarele fizice
    static const int P = 6;
    static const int nWorkers = 2 * P;

    //parametrii
    struct environment *staticEnv = (environment*)malloc(sizeof(struct environment));
    long int size = pow(10, k);
    staticEnv->matrix_a = a;
    staticEnv->matrix_b = b;
    int** c = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++)
        c[i] = (int*)malloc(size * sizeof(int));
    staticEnv->matrix_c = c;
    staticEnv->kValue = k;
    staticEnv->matrixIndex = index;

    long long int iterations = size * size;
    long long int threadIterations = iterations / nWorkers;
    int start = 0;
    int end = threadIterations;

    //thread-uri
    DWORD threads[nWorkers];
    HANDLE threadHandles[nWorkers];
    for (int i = 0; i < nWorkers; i++)
    {
        staticEnv->i = start;
        staticEnv->j = end;
        staticEnv->worker = i;
        threadHandles[i] = CreateThread(
            NULL,
            0,
            multiplicateMatrix,
            staticEnv,
            0,
            &threads[i]
        );

        if (threads[i] == NULL)
        {
            cout << "CreateThread error: " << GetLastError();
        }
        WaitForSingleObject(threadHandles[i], INFINITE);

        start = end;
        if (end + threadIterations < iterations)
        {
            end = end + threadIterations;
        }
        else {
            end = iterations;
        }
        if ((iterations % nWorkers) != 0 && i + 1 == nWorkers - 1)
            end = iterations;
    }
    WaitForMultipleObjects(nWorkers, threadHandles, TRUE, INFINITE);
    for (int i = 0; i < nWorkers; i++)
    {
        CloseHandle(threadHandles[i]);
    }
}

int** generateMatrix(int k, int matrixIndex, char name)
{
    char fileName[256];
    sprintf(fileName, "C:\\Facultate\\CSSO\\Week6\\date\\%d_%d_%c.txt", 
        k, matrixIndex, name);
    cout << "\nGenerating matrix: " << fileName;
    long long int size = pow(10, k);
    cout << "\nMatrix size: " << size<<'\n';
    int** matrix = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++)
        matrix[i] = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size * size; i++)
    {
        int row = i / size;
        int col = i % size;
        char buffer[256];
        matrix[row][col] = (rand() % (1000 - 0 + 1)) + 0;
        sprintf(buffer, "%d\n", matrix[row][col]);
        writeToFile(buffer, fileName);
    }
    return matrix;
}

void generateMatrixSet(int n)
{
    int nSets = 15;
    for (int i = 0; i < nSets; i++)
    {
        int** matrix_a = generateMatrix(n, i, 'A');
        int** matrix_b = generateMatrix(n, i, 'B');
        sequentialMul(matrix_a, matrix_b, pow(10, n), n, i);
        staticMul(matrix_a, matrix_b, n, i);
    }
}

void createEnvironment()
{
    srand(time(0));
    for (int i = 0; i < 5; i++)
    {
        generateMatrixSet(i + 1);
        cout << '\n';
    }
}

int main()
{
    createEnvironment();
}
