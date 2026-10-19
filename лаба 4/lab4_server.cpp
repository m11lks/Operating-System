#include <iostream>
#include <windows.h>

using namespace std;

HANDLE hPipe = INVALID_HANDLE_VALUE;
HANDLE hEvent = INVALID_HANDLE_VALUE;
OVERLAPPED ovrlap = OVERLAPPED();

void create_namedpipe () {
    hPipe = CreateNamedPipe (
        TEXT("\\\\.\\pipe\\npipe"),
        PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED, 
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        0,
        0,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        cout << "Error. Code: " << GetLastError () << "\n";
        return;
    }

    hEvent = CreateEvent (
        NULL,
        TRUE,
        FALSE,
        NULL
    );
    ovrlap.hEvent = hEvent;
    cout << "Pipe created successfully.\n";
}

void connect_func() {
    bool connect_pipe;

    connect_pipe = ConnectNamedPipe(hPipe, NULL);

    if (!connect_pipe) {
        connect_pipe = TRUE;    
    }
    
    if (connect_pipe) {
        cout << "Pipe connected successfully.\n";
    } else {
        cout << "Error. Code: " << GetLastError() << "\n";
    }
}


void write_data () {
    char data[100];
    bool result; 

    cout << "Enter data to write: ";
    cin.getline(data, 100);

    SecureZeroMemory(&ovrlap, sizeof(OVERLAPPED));
    ovrlap.hEvent = hEvent;

    result = WriteFile (
        hPipe,
        data,
        strlen(data),
        NULL,
        &ovrlap
    );

    if (!result) {
        DWORD err = GetLastError();
        if (err == ERROR_IO_PENDING) {
            cout << "Waiting for delivery.\n";
            WaitForSingleObject(ovrlap.hEvent, INFINITE);
            cout << "Write completed successfully.\n";
        } else {
            cout << "Error. Code: " << err << "\n";
        }
    } else {
        cout << "Write completed immediately.\n";
    }
}


int main() {
    int choice;

    do {
        cout << "Server Menu: " << endl;
        cout << "1. Create Named Pipe" << endl;
        cout << "2. Connection" << endl;
        cout << "3. Write data" << endl;
        cout << "4. Disconnection " << endl;
        cout << "5. Exit " << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                create_namedpipe();
                break;
            case 2:
                connect_func();
                break;
            case 3:
                write_data();
                break;
            case 4:
                DisconnectNamedPipe(hPipe);
                cout << "Server disconnected successfully.\n";
                break;
            case 5:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n" << endl;
        } 
    } while (choice != 5);

    if (hPipe != INVALID_HANDLE_VALUE) CloseHandle(hPipe);
    if (hEvent != INVALID_HANDLE_VALUE) CloseHandle(hEvent);
    return 0;
}
