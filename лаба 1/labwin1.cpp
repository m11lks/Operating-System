#define UNICODE
#define _UNICODE

#include <iostream>
#include <windows.h>
#include <locale.h>

using namespace std;

void ShowDrivesFirst()
{
    system("cls");
    DWORD drives = GetLogicalDrives();
    wcout<<L"Detected drives:"<<endl;
    for (int i=0; i<26; i++)
    {
        DWORD ind = 1<<i;
        if (ind & drives)
        {
            wcout<<(wchar_t)('A'+ i)<<L":\\\n";
        }
    }
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void ShowDrivesSecond()
{
    system("cls");
    TCHAR szBuffer[MAX_PATH];
    DWORD disks = MAX_PATH;
    DWORD res = GetLogicalDriveStrings(disks, szBuffer);
    LPTSTR disk = szBuffer;
    wcout<<L"Detected drives:"<<endl;
    while(*disk)
    {
        wcout<<disk<<endl;
        while (*disk) {
             disk++;
            }
             disk++;
    }
    wcout<<L"\nPress Enter...";
    wcin.get();
}


void DriveType()
{
    system("cls");
    TCHAR szDrive[MAX_PATH];
    wcout<<L"Enter the drive name (ex., C:\\): ";
    wcin.getline(szDrive, MAX_PATH);
    UINT type = GetDriveType(szDrive);
    wcout<<L"Drive "<<szDrive<<L" type: "<<endl;

    if (type == DRIVE_UNKNOWN) wcout<<L"UNKNOWN"<<endl;
    if (type == DRIVE_NO_ROOT_DIR) wcout<<L"DRIVE NO ROOT DIRECTORY"<<endl;
    if (type == DRIVE_REMOVABLE) wcout<<L"REMOVABLE"<<endl;
    if (type == DRIVE_FIXED) wcout<<L"FIXED"<<endl;
    if (type == DRIVE_REMOTE) wcout<<L"REMOTE"<<endl;
    if (type == DRIVE_CDROM) wcout<<L"CDROM"<<endl;
    if (type == DRIVE_RAMDISK) wcout<<L"RAMDISK"<<endl;
    
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void VolumeInfo()
{
    system("cls");
    TCHAR szDrive[MAX_PATH];
    wcout<<L"Enter drive path (ex., C:\\): ";
    wcin.getline(szDrive, MAX_PATH);

    TCHAR VolBuffer[MAX_PATH];
    TCHAR fsBuffer[MAX_PATH];
    DWORD SerialNumber;
    DWORD MaxLen;
    DWORD fsFlags;
    
    BOOL GetVolumeInformationFlag = GetVolumeInformation(
        szDrive,
        VolBuffer,
        MAX_PATH,
        &SerialNumber,
        &MaxLen,
        &fsFlags,
        fsBuffer,
        MAX_PATH
    );

    if (GetVolumeInformationFlag != 0)
    {
        wcout<<L"Volume Information: "<<endl;
        if (VolBuffer[0] == L'\0') 
            wcout<<L"Volume Name is [No Label]"<<endl;
        else 
            wcout<<L"Volume Name is "<<VolBuffer<< endl;

        wcout<<L"Volume Serial Number is "<<SerialNumber<<endl;
        wcout<<L"File System is "<<fsBuffer<<endl;

        wcout<<L"File System Flags: "<<fsFlags<<endl;

        if (fsFlags & FILE_FILE_COMPRESSION)
            wcout<<L"\tFILE_FILE_COMPRESSION (Compression Supported)"<<endl;
        if (fsFlags & FILE_VOLUME_IS_COMPRESSED)
            wcout<<L"\tFILE_VOLUME_IS_COMPRESSED"<<endl;
        if (fsFlags & FILE_CASE_PRESERVED_NAMES)
            wcout<<L"\tFILE_CASE_PRESERVED_NAMES (Case Preserved)"<<endl;
        if (fsFlags & FILE_UNICODE_ON_DISK)
            wcout<<L"\tFILE_UNICODE_ON_DISK (Unicode Support)"<<endl;
        if (fsFlags & FILE_PERSISTENT_ACLS)
            wcout<<L"\tFILE_PERSISTENT_ACLS (Access Control)"<<endl;
        if (fsFlags & FILE_CASE_SENSITIVE_SEARCH)
            wcout<<L"\tFILE_CASE_SENSITIVE_SEARCH (Case Sensitive)"<<endl;
        if (fsFlags & FILE_SUPPORTS_ENCRYPTION)
            wcout<<L"\tFILE_SUPPORTS_ENCRYPTION"<<endl;
        if (fsFlags & FILE_READ_ONLY_VOLUME)
            wcout<<L"\tFILE_READ_ONLY_VOLUME"<<endl;
}
    else wcout<<L"Not Present (Error code: "<< GetLastError()<<L")"<< endl;

    wcout<<L"\nPress Enter to continue...";
    wcin.get();
}

void FreeSpace()
{
    system("cls");
    TCHAR szDrive[MAX_PATH];
    wcout<<L"Enter drive path (ex., C:\\): ";
    wcin.getline(szDrive, MAX_PATH);

    DWORD sectorsPerCluster;
    DWORD bytesPerSector;
    DWORD numberOfFreeClusters;
    DWORD totalNumberOfClusters;
    double Clustersize;
    double TotalSpace;
    double Freespace;
    double UsedSpace;

    BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpace (
        szDrive,
        &sectorsPerCluster,
        &bytesPerSector,
        &numberOfFreeClusters,
        &totalNumberOfClusters
    );

    if (GetDiskFreeSpaceFlag != 0)
    {
        wcout<<L"Disk Free Space: "<<endl;
        wcout<<L"Sectors per Clusters: "<<sectorsPerCluster<<endl;
        wcout<<L"Bytes per Sectors: "<<bytesPerSector<<endl;

        Clustersize = (double)sectorsPerCluster * bytesPerSector;
        wcout<<L"Cluster size: "<<Clustersize<<L" bytes"<<endl;

        TotalSpace = Clustersize * (double)totalNumberOfClusters;
        Freespace = Clustersize * (double)numberOfFreeClusters;
    
        wcout<<L"Total space: "<<TotalSpace/(1024.0*1024.0)<<L" MB"<<L" = "<<TotalSpace/(1024.0*1024.0*1024.0)<<L" GB"<<endl;
        wcout<<L"Free space: "<<Freespace/(1024.0*1024.0)<<L" MB"<<L" = "<<Freespace/(1024.0*1024.0*1024.0)<<L" GB"<<endl;
        wcout<<L"Used Space: "<<(TotalSpace - Freespace)/(1024.0*1024.0)<<L" MB"<<L" = "<<(TotalSpace - Freespace)/(1024.0*1024.0*1024.0)<<L" GB"<<endl;
    }
    else wcout<<L"\nError. Code: "<<GetLastError()<<endl;
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void NewDirectory()
{
    system("cls");
    TCHAR szPath[MAX_PATH];
    wcout<<L"Enter path for the new directory (ex., C:\\NewFolder): ";
    wcin.getline(szPath, MAX_PATH);

    if (CreateDirectory(szPath,NULL))
    {
        wcout<<L"The new directory created successfully."<<endl;
    }
    else wcout<<L"\nError. Code: "<<GetLastError()<<endl;

    wcout<<L"\nPress Enter...";
    wcin.get();
}

void RemoveNewDir()
{
    system("cls");
    TCHAR szPath[MAX_PATH];
    wcout<<L"Enter path for the directory you want to remove (ex., C:\\NewFolder): ";
    wcin.getline(szPath, MAX_PATH);

    if (RemoveDirectory(szPath))
    {
        wcout<<L"The directory removed successfully."<<endl;
    }
    else wcout<<L"\nError. Code: "<<GetLastError()<<endl;

    wcout<<L"\nPress Enter...";
    wcin.get();
}

void NewFile()
{
    system("cls");
    TCHAR szFileName[MAX_PATH];
    wcout<<L"Enter file name you want to create (ex., NewFile.txt): ";
    wcin.getline(szFileName,MAX_PATH);

    HANDLE NFile = CreateFile(
        szFileName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (NFile == INVALID_HANDLE_VALUE)
    {
        wcout<<L"Error. Code: "<<GetLastError()<<endl;
    }
    else
    {
        wcout<<L"File created successfully."<<endl;
        CloseHandle(NFile);
    }
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void CpFile()
{
    system("cls");
    TCHAR exfile[MAX_PATH];
    TCHAR fpath[MAX_PATH];

    wcout<<L"Enter the file name (ex., C:\\NewFile.txt): ";
    wcin.getline(exfile,MAX_PATH);
    wcout<<L"Enter the destination path (ex., D:\\NewFile.txt: ";
    wcin.getline(fpath,MAX_PATH);

    if (CopyFile(exfile, fpath, TRUE)) {
        wcout<<L"File copied successfully."<<endl;
    }
    else wcout<<L"Error. Code: "<<GetLastError()<<endl;
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void MvFile()
{
    system("cls");
    TCHAR exfile[MAX_PATH];
    TCHAR fpath[MAX_PATH];

    wcout<<L"Enter the file name: ";
    wcin.getline(exfile,MAX_PATH);
    wcout<<L"Enter the destination path: ";
    wcin.getline(fpath,MAX_PATH);

    if (MoveFile(exfile, fpath)) {
        wcout<<L"File moved successfully."<<endl;
    }
    else wcout<<L"Error. Code: "<<GetLastError()<<endl;
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void MvFileEx()
{
    system("cls");
    TCHAR exfile[MAX_PATH];
    TCHAR fpath[MAX_PATH];

    wcout<<L"Enter the file name: ";
    wcin.getline(exfile,MAX_PATH);
    wcout<<L"Enter the destination path: ";
    wcin.getline(fpath,MAX_PATH);

    if (MoveFileEx(exfile, fpath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        wcout<<L"File moved successfully."<<endl;
    }
    else wcout<<L"Error. Code: "<<GetLastError()<<endl;
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void GFileAtt()
{
    system("cls");
    TCHAR nfile[MAX_PATH];
    wcout<<L"Enter the file name (ex., C:\\NewFile.txt): ";
    wcin.getline(nfile, MAX_PATH);

    DWORD attr = GetFileAttributes(nfile);

    if (attr == INVALID_FILE_ATTRIBUTES) {
        wcout << L"Error. File not found. Code: " << GetLastError() << endl;
        wcout << L"\nPress Enter...";
        wcin.get();
        return; 
    }

    wcout<<L"File Attributes: "<<endl;

    if (attr & FILE_ATTRIBUTE_ARCHIVE) {
        wcout<<L"\t-Archive"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_SYSTEM) {
        wcout<<L"\t-System"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_HIDDEN) {
        wcout<<L"\t-Hidden"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_READONLY) {
        wcout<<L"\t-Only for reading"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_TEMPORARY) {
        wcout<<L"\t-Temporary"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_COMPRESSED) {
        wcout<<L"\t-Compressed"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_ENCRYPTED) {
        wcout<<L"\t-Encrypted"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_SPARSE_FILE) {
        wcout<<L"\t-Sparce file"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        wcout<<L"\t-Directory"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_DEVICE) {
        wcout<<L"\t-System reserved"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_OFFLINE) {
        wcout<<L"\t-Offline"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) {
        wcout<<L"\t-Not content indexed"<<endl;
    }
    if (attr & FILE_ATTRIBUTE_REPARSE_POINT) {
        wcout<<L"\t-Reparse point"<<endl;
    }

    if (attr & FILE_ATTRIBUTE_NORMAL) {
        wcout<<L"\t-Normal"<<endl;
    }

    wcout<<L"\nPress Enter...";
    wcin.get();
}

void SFileAtt()
{
    system("cls");
    TCHAR nfile[MAX_PATH];
    wcout<<L"Enter the file name (ex., C:\\NewFile.txt): ";
    wcin.getline(nfile, MAX_PATH);

    DWORD attr = GetFileAttributes(nfile);

    if (attr == INVALID_FILE_ATTRIBUTES) {
        wcout<<L"Error. File not found. Code: "<<GetLastError()<<endl;
        wcout<<L"\nPress Enter...";
        wcin.get();
        return; 
    }

    wcout<<L"Choose the option: "<<endl;
    wcout<<L"1. Hidden"<<endl;
    wcout<<L"2. Read Only"<<endl;
    wcout<<L"3. System"<<endl;
    wcout<<L"4. Archive"<<endl;
    wcout<<L"5. Normal"<<endl;

    int choice;
    wcin>>choice;
    wcin.ignore(1000, L'\n');

    switch (choice) {
        case 1: attr ^= FILE_ATTRIBUTE_HIDDEN; break;
        case 2: attr ^= FILE_ATTRIBUTE_READONLY; break;
        case 3: attr ^= FILE_ATTRIBUTE_SYSTEM; break;
        case 4: attr ^= FILE_ATTRIBUTE_ARCHIVE; break;
        case 5: attr = FILE_ATTRIBUTE_NORMAL; break;
        default: return;
    }

    if (SetFileAttributes(nfile, attr)) {
        wcout<<L"Attributes updated successfully."<<endl;
    } else {
        wcout<<L"Error. Code: "<<GetLastError()<<endl;
    }

    wcout<<L"\nPress Enter...";
    wcin.get();
}

void InfoHandle() 
{
    system("cls");
    TCHAR szFileName[MAX_PATH];
    wcout<<L"Enter the file name (ex., C:\\NewFile.txt): ";
    wcin.getline(szFileName,MAX_PATH);

    HANDLE NFile = CreateFile(
        szFileName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (NFile == INVALID_HANDLE_VALUE)
    {
        wcout<<L"Error. Code: "<<GetLastError()<<endl;
    }
    else
    {
        BY_HANDLE_FILE_INFORMATION bhfi;
        if (GetFileInformationByHandle(NFile, &bhfi)) {
            wcout << L"File Information by Handle:" << endl;
            wcout << L"\t- Volume Serial Number: " << bhfi.dwVolumeSerialNumber << endl;
            wcout << L"\t- File Index: " << bhfi.nFileIndexHigh << L":" << bhfi.nFileIndexLow << endl;
            wcout << L"\t- Number of Links: " << bhfi.nNumberOfLinks << endl;
            wcout << L"\t- File Size: " << bhfi.nFileSizeHigh << L" : " << bhfi.nFileSizeLow << L" bytes" << endl;
        } 
        else {
            wcout << L"Error. Code: " << GetLastError() << endl;
        }
        CloseHandle(NFile);
     }
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void GFileTime()
{
    system("cls");
    TCHAR szFileName[MAX_PATH];
    wcout<<L"Enter the file name (ex., C:\\NewFile.txt): ";
    wcin.getline(szFileName,MAX_PATH);

    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;

    HANDLE hFile = CreateFile(
        szFileName, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        wcout<<L"Error opening file. Code: "<<GetLastError()<<endl;
    }
    else{
        if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
            wcout<<L"File Time Information:"<< endl;

            FileTimeToSystemTime(&ftCreate, &stUTC);
            SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
            wcout<<L"\t- Created: "<<stLocal.wDay<< L"."<<stLocal.wMonth<<L"."<<stLocal.wYear<<L" "<<stLocal.wHour<< L":"<<stLocal.wMinute<<endl;

            FileTimeToSystemTime(&ftAccess, &stUTC);
            SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
            wcout<<L"\t- Last Access: "<<stLocal.wDay<< L"."<<stLocal.wMonth<< L"."<<stLocal.wYear<< L" "<< stLocal.wHour<< L":"<<stLocal.wMinute<<endl;

            FileTimeToSystemTime(&ftWrite, &stUTC);
            SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
            wcout<<L"\t- Last Modified: "<<stLocal.wDay<<L"."<<stLocal.wMonth<<L"."<<stLocal.wYear<<L" "<<stLocal.wHour<<L":"<<stLocal.wMinute<<endl;

    }
        else {
            wcout << L"Error getting time. Code: " << GetLastError() << endl;
    }
        CloseHandle(hFile);
    }
    wcout<<L"\nPress Enter...";
    wcin.get();
}

void SFileTime()
{
    system("cls");
    TCHAR szFileName[MAX_PATH];
    wcout << L"Enter the file name (ex., C:\\NewFile.txt): ";
    wcin.getline(szFileName, MAX_PATH);

    HANDLE hFile = CreateFile(
        szFileName, 
        FILE_WRITE_ATTRIBUTES, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        wcout<<L"Error. Code: "<<GetLastError()<<endl;
    }
    else {
        SYSTEMTIME st;
        FILETIME ft;

        GetSystemTime(&st); 
        SystemTimeToFileTime(&st, &ft);

        if (SetFileTime(hFile, &ft, &ft, &ft)) {
            wcout<<L"File time updated to current system time successfully."<<endl;
        }
        else {
            wcout<<L"Error. Code: "<<GetLastError()<<endl;
        }

        CloseHandle(hFile);
    }
    wcout << L"\nPress Enter...";
    wcin.get();
}

void FileMenu()
{
    system("cls");
    int choice;

    wcout<<L"Choose the function:"<<endl;
    wcout<<L"1. CopyFile"<<endl;
    wcout<<L"2. MoveFile"<<endl;
    wcout<<L"3. MoveFileEx"<<endl;
    wcin>>choice;
    wcin.ignore(1000, L'\n');

    switch(choice)
    {
        case 1: CpFile(); break;
        case 2: MvFile(); break;
        case 3: MvFileEx(); break;
    }

}

void ShowDisksMenu()
{
    system("cls");
    int choice;

    wcout<<L"Choose the function:"<<endl;
    wcout<<L"1. GetLogicalDrives"<<endl;
    wcout<<L"2. GetLogicalDriveStrings"<<endl;
    wcin>>choice;
    wcin.ignore(1000, L'\n');

    switch(choice)
    {
        case 1: ShowDrivesFirst(); break;
        case 2: ShowDrivesSecond(); break;
    }
  
}

void DriveInfoMenu()
{
    system("cls");
    int choice;

    wcout<<L"Choose the function:"<<endl;
    wcout<<L"1. GetDriveType"<<endl;
    wcout<<L"2. GetVolumeInformation"<<endl;
    wcout<<L"3. GetDiskFreeSpace"<<endl;
    wcin>>choice;
    wcin.ignore(1000, L'\n');

    switch(choice)
    {
        case 1: DriveType(); break;
        case 2: VolumeInfo(); break;
        case 3: FreeSpace(); break;
    }
}

void DirectoryMenu()
{
    system("cls");
    int choice;

    wcout<<L"Choose the function:"<<endl;
    wcout<<L"1. CreateDirectory"<<endl;
    wcout<<L"2. RemoveDirectory"<<endl;
    wcin>>choice;
    wcin.ignore(1000, L'\n');

    switch(choice)
    {
        case 1: NewDirectory(); break;
        case 2: RemoveNewDir(); break;
    }
}

void LastMenu()
{
    system("cls");
    int choice;

    wcout<<L"Choose the function:"<<endl;
    wcout<<L"1. GetFileAttributes"<<endl;
    wcout<<L"2. SetFileAttributes"<<endl;
    wcout<<L"3. GetFileInformattionByHandle"<<endl;
    wcout<<L"4. GetFileTime"<<endl;
    wcout<<L"5. SetFileTime"<<endl;
    wcin>>choice;
    wcin.ignore(1000, L'\n');

    switch(choice)
    {
        case 1: GFileAtt(); break;
        case 2: SFileAtt(); break;
        case 3: InfoHandle(); break;
        case 4: GFileTime(); break;
        case 5: SFileTime(); break;
    }
}



int main() 
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    setlocale(LC_ALL, "Russian");
    int choice;
    
    do {
        system("cls");
        wcout<<L"Menu"<<endl;
        wcout<<L"1 - List of the drives"<<endl;
        wcout<<L"2 - Drive information"<<endl;
        wcout<<L"3 - Directory manipulations"<<endl;
        wcout<<L"4 - Create a new file"<<endl;
        wcout<<L"5 - File manipulations"<<endl;
        wcout<<L"6 - Analysis and modification of file attributes"<<endl;
        wcout<<L"0 - Exit"<<endl;
        wcin>>choice;
        wcin.ignore(1000, L'\n');

        switch(choice)
        {
            case 1: ShowDisksMenu(); break;
            case 2: DriveInfoMenu(); break;
            case 3: DirectoryMenu(); break;
            case 4: NewFile(); break;
            case 5: FileMenu(); break;
            case 6: LastMenu(); break;
        }
    } while(choice!=0);
    return 0;
}
