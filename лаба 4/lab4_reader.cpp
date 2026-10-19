#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

#pragma comment(lib, "winmm.lib") 

using namespace std;

const int num_pages = 12;                        
const int page_size = 4096;                  
const int total_size = page_size * num_pages; 
const char* mutex_name = "Local\\mutexn";
const char* map_name = "Local\\mapping_file";

void LogWrite(HANDLE hLog, string data) {
    data += "\r\n"; 
    WriteFile(hLog, data.c_str(), data.length(), NULL, NULL);
}

int main() {
    srand((unsigned)GetCurrentProcessId() ^ timeGetTime());

    string log_name = "reader_" + to_string(GetCurrentProcessId()) + ".log";
    HANDLE hLog = CreateFileA(log_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    HANDLE read_sem[num_pages];
    HANDLE write_sem[num_pages];
    DWORD target;
    bool open_success = true;
    for (int i = 0; i < num_pages; ++i) {
        string write_name = "Local\\sema_write_" + to_string(i);
        string read_name = "Local\\sema_read_" + to_string(i);  
        string mutex_name = "Local\\mutex_page_" + to_string(i);
        read_sem[i] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, read_name.c_str());
        write_sem[i] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, write_name.c_str());

        if (!write_sem[i]|| !read_sem [i]) open_success = false;
        
    }

    HANDLE hMapF = OpenFileMappingA(FILE_MAP_READ, FALSE, map_name);

    if (!hMapF || !open_success) {
        cerr << "Error: You must run at least one Writer process first!\n";
        return 1;
    }

    LPVOID pBuf = MapViewOfFile(hMapF, FILE_MAP_READ, 0, 0, total_size);
    VirtualLock(pBuf, total_size); 

    LogWrite(hLog, "Reader started (PID: " + to_string(GetCurrentProcessId()) + ")");

    for (int i = 0; i < 12; ++i) {
        LogWrite(hLog, "[" + to_string(timeGetTime()) + "] State: Start waiting");
        
        DWORD wait_result = WaitForMultipleObjects(num_pages, read_sem, FALSE, INFINITE);
        target = wait_result - WAIT_OBJECT_0;

        LogWrite(hLog, "[" + to_string(timeGetTime()) + "] State: Reading | Page: " + to_string(target));

        char* page_ptr = (char*)pBuf + (target * page_size);
        string shared_data(page_ptr); 
    
        Sleep(500 + rand() % 1001);

        LogWrite(hLog, "[" + to_string(timeGetTime()) + "] State: Moving to release");
        
        ReleaseSemaphore(write_sem[target], 1, NULL); 
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