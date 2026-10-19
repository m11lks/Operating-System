#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

HANDLE StartProcess(const string& exeName) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmdLine[256];
    strcpy_s(cmdLine, exeName.c_str());
    if (!CreateProcessA(
            NULL,               
            cmdLine,            
            NULL,               
            NULL,               
            FALSE,              
            CREATE_NO_WINDOW,   
            NULL,               
            NULL,               
            &si,                
            &pi                 
        )) 
    {
        cerr << "Failed to start: " << exeName << " (Error: " << GetLastError() << ")\n";
        return NULL;
    }

    CloseHandle(pi.hThread);
    
    return pi.hProcess;
}

int main() {
    int numWriters = 0;
    int numReaders = 0;

    cout << "Enter number of Writers to launch: ";
    cin >> numWriters;
    cout << "Enter number of Readers to launch: ";
    cin >> numReaders;

    vector<HANDLE> processHandles;

    cout << "\n[Dispatcher] Launching processes in background...\n";

    for (int i = 0; i < numWriters; ++i) {
        HANDLE hProc = StartProcess("lab4_writer.exe");
        if (hProc) processHandles.push_back(hProc);
    }


    for (int i = 0; i < numReaders; ++i) {
        HANDLE hProc = StartProcess("lab4_reader.exe");
        if (hProc) processHandles.push_back(hProc);
    }

    cout << "[Dispatcher] Successfully launched " << processHandles.size() << " processes.\n";
    cout << "[Dispatcher] They are working silently. Check the *.log files in this folder.\n";
    cout << "[Dispatcher] Waiting for all processes to finish...\n";

    if (!processHandles.empty()) {
        WaitForMultipleObjects(
            static_cast<DWORD>(processHandles.size()), 
            processHandles.data(), 
            TRUE, 
            INFINITE
        );
    }

    for (HANDLE hProc : processHandles) {
        CloseHandle(hProc);
    }

    cout << "\n[Dispatcher] All processes have finished their work successfully!\n";
    system("pause");

    return 0;
}
