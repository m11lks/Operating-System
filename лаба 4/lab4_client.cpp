#include <iostream>
#include <windows.h>

using namespace std;

HANDLE hFile = INVALID_HANDLE_VALUE;
OVERLAPPED ovrlap = OVERLAPPED();
char buffer[100];

VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    if (!dwErrorCode) {
        cout << "Read successfully.\n";
        cout << "Data received: " << buffer << "\n";
    }
    else {
        cout << "Error. Code: " << GetLastError () << "\n";
    }
}

void connect_to_pipe () {
    hFile = CreateFile (
        TEXT("\\\\.\\pipe\\npipe"),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "Error. Code: " << GetLastError () << "\n";
        return;
    } 

    cout << "Connected to server successfully.\n";

}

void read_data () {
    bool result;

    memset(buffer, 0, sizeof(buffer));
    SecureZeroMemory(&ovrlap, sizeof(OVERLAPPED));

    result = ReadFileEx (
        hFile,
        buffer,
        sizeof(buffer),
        &ovrlap,
        FileIOCompletionRoutine
    );

    if (result) {
        cout << "Asynchronous read operation requested.\n";
        SleepEx(INFINITE, TRUE); 
    } else {
        cout << "Error. Code: " << GetLastError() << "\n";
    }
}


int main() {
    int choice;

    do {
        cout << "Client Menu: " << endl;
        cout << "1. Connect to pipe" << endl;
        cout << "2. Read data" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                connect_to_pipe();
                break;
            case 2:
                read_data();
                break;
            case 3:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n" << endl;
        } 
    } while (choice != 3);

    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
    return 0;
}
