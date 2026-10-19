#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

#pragma comment(lib, "winmm.lib") 

using namespace std;

const int num_pages = 12;                        
const int page_size = 4096;                  
const int total_size = page_size * num_pages; 
const char* map_name = "Local\\mapping_file";

void LogWrite(HANDLE hLog, string data) {
    data += "\r\n"; 
    WriteFile(hLog, data.c_str(), (DWORD)data.length(), NULL, NULL); 
}

int main() {
    srand((unsigned)GetCurrentProcessId() ^ timeGetTime());

    string log_name = "writer_" + to_string(GetCurrentProcessId()) + ".log";
    HANDLE hLog = CreateFileA(log_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE write_sem[num_pages];
    HANDLE read_sem[num_pages];
    DWORD target;
    bool open_success = true;
    for (int i = 0; i < num_pages; ++i) {
        string write_name = "Local\\sema_write_" + to_string(i);
        string read_name = "Local\\sema_read_" + to_string(i);  
        string mutex_name = "Local\\mutex_page_" + to_string(i);
        read_sem[i] = CreateSemaphoreA(NULL, 0, 1, read_name.c_str());
        write_sem[i] = CreateSemaphoreA(NULL, 1, 1, write_name.c_str());
        if (!write_sem [i] || !read_sem [i]) open_success = false; 
    }

    HANDLE hMapF = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)total_size, map_name);
    if (!hMapF) {
        cerr << "Error: Could not create file mapping!\n";
        return 1;
    }

    LPVOID pBuf = MapViewOfFile(hMapF, FILE_MAP_ALL_ACCESS, 0, 0, total_size);
    VirtualLock(pBuf, total_size); 

    LogWrite(hLog, "Writer started (PID: " + to_string(GetCurrentProcessId()) + ")");

    for (int i = 0; i < 12; ++i) {
        LogWrite(hLog, "[" + to_string(timeGetTime()) + "] State: Start waiting");
    
        DWORD wait_result = WaitForMultipleObjects(num_pages, write_sem, FALSE, INFINITE);
        
        target = wait_result - WAIT_OBJECT_0;

        LogWrite(hLog, "[" + to_string(timeGetTime()) + "] State: Writing | Page: " + to_string(target));
        
        char* pagePtr = (char*)pBuf + (target * page_size);
        string message = "Data from Writer PID " + to_string(GetCurrentProcessId()) + ", Iteration " + to_string(i);
        memcpy(pagePtr, message.c_str(), message.length() + 1);

        Sleep(500 + rand() % 1001);

        LogWrite(hLog, "[" + to_string(timeGetTime()) + "] State: Moving to release");
        
        ReleaseSemaphore(read_sem[target], 1, NULL); 
        Sleep(200); 
    }

    VirtualUnlock(pBuf, total_size);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapF);
    for (int i = 0; i < num_pages; ++i) {
        CloseHandle(write_sem[i]);
        CloseHandle(read_sem[i]);
    }
    CloseHandle(hLog);

    return 0;
}