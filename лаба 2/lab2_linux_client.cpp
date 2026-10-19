#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <unistd.h>
#include <cstring>

#define FILENAME "n_file.dat"
#define FILESIZE 4096

using namespace std;

int fd = -1;
char* ptr = nullptr;

void mapping_file() {
    if (ptr != nullptr) {
        cout << "Mapping already performed." << endl;
        return;
    }

    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        perror("open");
        return;
    }

    ptr = (char*)mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        ptr = nullptr;
        return;
    }
    cout << "File mapped successfully." << endl;
}

void read_data() {
    if (ptr == nullptr || fd == -1) {
        cout << "Error: You must map the file before reading." << endl;
        return;
    }

    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    cout << "Waiting for data." << endl;
    int result = select(fd + 1, &read_fds, NULL, NULL, &timeout);

    if (result == -1) {
        perror("select");
    } else if (result == 0) {
        cout << "Timeout: No data available to read." << endl;
    } else {
        cout << "Client received: " << ptr << endl;
    }
}

void finish_work() {
    if (ptr != nullptr && ptr != MAP_FAILED) {
        munmap(ptr, FILESIZE);
    }

    if (fd != -1) {
        close(fd);
    }
    cout << "Memory unmapped. Client closing." << endl;
}

int main() {
    int choice;

    while (true) {
        cout << "Client Menu: " << endl;
        cout << "1. Perform mapping" << endl;
        cout << "2. Read data" << endl;
        cout << "3. Finish" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> choice)) break;

        switch (choice) {
            case 1:
                mapping_file();
                break;
            case 2:
                read_data();
                break;
            case 3:
                finish_work();
                return 0;
            default:
                cout << "Invalid choice. Try again." << endl;
        }
    }
    return 0;
}