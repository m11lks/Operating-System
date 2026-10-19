#include <iostream>
#include <fcntl.h>      
#include <sys/mman.h>   
#include <unistd.h>     
#include <cstring>      

#define FILENAME "n_file.dat"
#define FILESIZE 4096

using namespace std;

int fd = -1;
char* ptr = nullptr;

 void mapping_file() {

    if (ptr != NULL) {
        cout << "Mapping already performed." << endl;
        return;
    }

    fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) { perror("open"); return; }
    ftruncate(fd, FILESIZE);
                
    ptr = (char*)mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) { 
        perror("mmap"); 
        close(fd); 
        ptr = nullptr;
        return; 
    }
    cout << "File mapped successfully." << endl;
 }


void write_data() {
    if (ptr != nullptr) {
        sprintf(ptr, "Hello, shared memory!");
        cout << "Data written to shared memory: " << ptr << endl;
    } 
    else {
        cout << "Error: You must map the file before writing." << endl;
    }
}

void finish_work() {
    if (ptr != nullptr && ptr != MAP_FAILED) {
        munmap(ptr, FILESIZE);
    }
    
    if (fd != -1) {
        close(fd);
    }

    unlink(FILENAME);
    cout << "Complete: Unmapped memory and deleted file." << endl;
}


int main() {
    int choice;

    while (true) {

        cout << "Server Menu: " << endl;
        cout << "1. Perform mapping" << endl;
        cout << "2. Write data" << endl;
        cout << "3. Finish" << endl;
        cout << "Enter your choice: ";
        
        if (!(cin >> choice)) {
            break; 
        }

        switch (choice) {
            case 1:
                mapping_file();
                break;
            case 2:
                write_data();
                break;
            case 3:
                finish_work();
                cout << "Exiting server application." << endl;
                return 0;
            default:
                cout << "Invalid choice. Try again." << endl;
        }
    }
    return 0;
}











