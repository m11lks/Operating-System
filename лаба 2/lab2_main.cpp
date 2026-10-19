#include <iostream>
#include <windows.h>
#include <iomanip>
#include <string>
#include <stdlib.h>

using namespace std;

void sys_info()
{
    system("cls");
    SYSTEM_INFO sinf;
    GetSystemInfo(&sinf);
    cout << "System Info:" << endl;
    cout << "Page Size: " << sinf.dwPageSize << " bytes" << endl;
    cout << "Processor Type: " << sinf.dwProcessorType << endl;
    cout << "Number of Processors: " << sinf.dwNumberOfProcessors << endl;
    cout << "Active Processor Mask: " << sinf.dwActiveProcessorMask << endl;
    cout << "Minimum Application Address: " << sinf.lpMinimumApplicationAddress << endl;
    cout << "Maximum Application Address: " << sinf.lpMaximumApplicationAddress << endl;    
}

void memory_status() {

    system("cls");
    MEMORYSTATUS ms;
    GlobalMemoryStatus(&ms);

    cout << "Memory Status: " << endl;
    cout << "Memory Load: " << ms.dwMemoryLoad << "%" << endl;
    cout << "Total Physical: " << ms.dwTotalPhys / (1024 * 1024) << " MB" << endl;
    cout << "Available Physical: " << ms.dwAvailPhys / (1024 * 1024) << " MB" << endl;
    cout << "Total Page File: " << ms.dwTotalPageFile / (1024 * 1024) << " MB" << endl;
    cout << "Available Page File: " << ms.dwAvailPageFile / (1024 * 1024) << " MB" << endl;
    cout << "Total Virtual: " << ms.dwTotalVirtual / (1024 * 1024) << " MB" << endl;
    cout << "Available Virtual: " << ms.dwAvailVirtual / (1024 * 1024) << " MB" << endl;
}

void query_state() {

    system("cls");
    LPVOID addrs;
    cout << "Enter memory address to query (hex): ";
    cin >> hex >> addrs;

    MEMORY_BASIC_INFORMATION basic_info;
    if (VirtualQuery(addrs, &basic_info, sizeof(basic_info))) {
        cout << "Memory Info:" << endl;
        cout << "Base Address: " << basic_info.BaseAddress << endl; 
        cout << "Allocation Base: " << basic_info.AllocationBase << endl; 
        cout << "Region Size: " << dec << basic_info.RegionSize << " bytes" << endl;
        
        cout << "State:              ";
        if (basic_info.State == MEM_COMMIT) cout << "MEM_COMMIT (Committed)";
        else if (basic_info.State == MEM_RESERVE) cout << "MEM_RESERVE (Reserved)";
        else if (basic_info.State == MEM_FREE)    cout << "MEM_FREE (Free)";
        cout << endl;

        cout << "Type:               ";
        if (basic_info.Type == MEM_IMAGE)       cout << "MEM_IMAGE (Executable image)";
        else if (basic_info.Type == MEM_MAPPED) cout << "MEM_MAPPED (Mapped file)";
        else if (basic_info.Type == MEM_PRIVATE) cout << "MEM_PRIVATE (Private memory)";
        else cout << "Unknown";
        cout << endl;

        cout << "Protection: 0x" << hex << basic_info.Protect << dec << endl;
    } 
    else {
        cout << "Error. Code: " << GetLastError() << endl;
    }
}

void allocate_memory() {

    system("cls");
    LPVOID address = NULL;
    SIZE_T size;
    int type_choice, addr_choice;

    cout << "Choose the function: " << endl;
    cout << "1. Separate" << endl;
    cout << "2. Simultaneous" << endl;
    cout << "Choice: ";
    cin >> type_choice;

    cout << "1. Automatic address" << endl;
    cout << "2. Manual address" << endl;
    cout << "Choice: ";
    cin >> addr_choice;

    if (addr_choice == 2) {
        cout << "Enter start address (hex): ";
        cin >> hex >> address;
    }

    cout << "Enter size in bytes (dec): ";
    cin >> dec >> size;

    LPVOID ptr = NULL;

    if (type_choice == 1) {
        ptr = VirtualAlloc(address, size, MEM_RESERVE, PAGE_READWRITE);
        if (ptr) {
            cout << "Region reserved at: " << ptr << endl;
            cout << "Press ENTER to commit physical memory...";
            cin.ignore();
            cin.get();
            if (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE)) {
                cout << "Memory committed." << endl;
            }
        }
    } else {
        ptr = VirtualAlloc(address, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (ptr) {
            cout << "Memory reserved and committed at: " << ptr << endl;
        }
    }

    if (!ptr) {
        cout << "Error. Code: " << GetLastError() << endl;
        return;
    }

    cout << "Free memory now? (1 - Yes, 0 - No): ";
    int free_choice;
    cin >> free_choice;
    if (free_choice == 1) {
        if (VirtualFree(ptr, 0, MEM_RELEASE)) {
            cout << "Memory released successfully." << endl;
        } else {
            cout << "Error. Code: " << GetLastError() << endl;
        }
    }
}


void write_memory_data() {

    system("cls");
    LPVOID address;
    int value;

    cout << "Enter address to write (hex): ";
    cin >> hex >> address;
    cout << "Enter value (int): ";
    cin >> dec >> value;

    MEMORY_BASIC_INFORMATION mbi;
    
    if (VirtualQuery(address, &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT) {
            if (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE) {
                int* ptr = (int*)address;
                *ptr = value;
                cout << "Successfully written." << endl;
            } 
            else {
                cout << "Error. Code: " << GetLastError() << endl;
                cout << "Current Protection Code: 0x" << hex << mbi.Protect << dec << endl;
            }

        } 
        else {
            cout << "Error. Code: " << GetLastError() << endl;
            if (mbi.State == MEM_RESERVE) {
                cout << "Current State: MEM_RESERVE" << endl;
            } 
            else if (mbi.State == MEM_FREE) {
                cout << "Current State: MEM_FREE" << endl;
            }
        }
    } 
    else {
        cout << "Error. Code: " << GetLastError() << endl;
    }
}

void protect_memory() {

    system("cls");
    LPVOID address;
    SIZE_T size;
    DWORD new_prot, old_prot;
    int choice;

    cout << "Enter address (hex): ";
    cin >> hex >> address;
    cout << "Enter size (dec): ";
    cin >> dec >> size;

    cout << "Choose protection:" << endl;
    cout << "1. PAGE_NOACCESS" << endl;
    cout << "2. PAGE_READONLY" << endl;
    cout << "3. PAGE_READWRITE" << endl;
    cout << "4. PAGE_EXECUTE" << endl;
    cout << "5. PAGE_EXECUTE_READ" << endl;
    cout << "6. PAGE_EXECUTE_READWRITE" << endl;
    cout << "Choice: ";
    cin >> dec >> choice;

    if (choice == 1) {
        new_prot = PAGE_NOACCESS;
    } else if (choice == 2) {
        new_prot = PAGE_READONLY;
    } else if (choice == 3) {
        new_prot = PAGE_READWRITE;
    } else if (choice == 4) {
        new_prot = PAGE_EXECUTE;
    } else if (choice == 5) {
        new_prot = PAGE_EXECUTE_READ;
    } else if (choice == 6) {
        new_prot = PAGE_EXECUTE_READWRITE;
    } else {
        new_prot = PAGE_NOACCESS; 
    }

    if (VirtualProtect(address, size, new_prot, &old_prot)) {
        cout << "Protection changed successfully." << endl;
        cout << "Old protection: 0x" << hex << old_prot << dec << endl;

        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(address, &mbi, sizeof(mbi))) {
            
            if (mbi.Protect == PAGE_NOACCESS) {
                cout << "Test: Access is BLOCKED (PAGE_NOACCESS working)." << endl;
            } 
            else if (mbi.Protect == PAGE_READONLY) {
                cout << "Test: Access is READ-ONLY." << endl;
            } 
            else if (mbi.Protect == PAGE_READWRITE) {
                cout << "Test: Access is FULL (READ/WRITE working)." << endl;
            }
        }
    } 
    else {
        cout << "Error. Code: " << GetLastError() << endl;
    }
}

void free_memory() {

    system("cls"); 
    LPVOID address;
    cout << "Enter address to free (hex): ";
    cin >> hex >> address;

    if (VirtualFree(address, 0, MEM_RELEASE)) {
        cout << "Memory at " << address << " successfully released." << endl;
    } else {
        cout << "Error. Code: " << GetLastError() << endl;
    }
}

int main() {

    int choice;
    while (true) {
        system("cls");
        cout << "Menu: " << endl;
        cout << "1. System Information" << endl;
        cout << "2. Global Memory Status" << endl;
        cout << "3. Query Memory Address State" << endl;
        cout << "4. Allocate Memory" << endl;
        cout << "5. Write Data to Memory" << endl;
        cout << "6. Set Protection" << endl;
        cout << "7. Free Memory" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> dec >> choice;

        if (choice == 0) {
            cout << "Exiting program..." << endl;
            break;
        }
        cout << endl;
        switch (choice) {
            case 1:
                sys_info();
                break;
            case 2:
                memory_status();
                break;
            case 3:
                query_state();
                break;
            case 4:
                allocate_memory();
                break;
            case 5:
                write_memory_data();
                break;
            case 6:
                protect_memory();
                break;
            case 7:
                free_memory();
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
                break;
        }
        
        cout << "Press ENTER to continue...";
        cin.ignore(1000, '\n'); 
        cin.get();
    }

    return 0;
}
