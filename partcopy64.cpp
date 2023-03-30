#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <Windows.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <source_path> <dest_path> <start_position> <length>" << endl;
        return 1;
    }

    // Parse the command-line arguments
    string sourcePath = argv[1];
    string destPath = argv[2];
    uint64_t startPosition = stoull(argv[3]);
    uint64_t length = stoull(argv[4]);

    // Open the source file
    ifstream sourceFile(sourcePath, ios::binary);
    if (!sourceFile.is_open()) {
        cerr << "Failed to open source file." << endl;
        return 1;
    }

    // Open the destination file (the physical drive)
    HANDLE hDrive = CreateFile(destPath.c_str(),
        GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL, OPEN_EXISTING, 0, NULL);
    if (hDrive == INVALID_HANDLE_VALUE) {
        cerr << "Failed to open physical drive." << endl;
        sourceFile.close();
        return 1;
    }

    // Set the file pointer to the desired start position
    DWORD dwPtr = SetFilePointer(hDrive, startPosition, NULL, FILE_BEGIN);
    if (dwPtr == INVALID_SET_FILE_POINTER) {
        cerr << "Failed to set file pointer to desired start position." << endl;
        CloseHandle(hDrive);
        sourceFile.close();
        return 1;
    }

    // Write the desired number of bytes to the drive
    char* buffer = new char[length];
    sourceFile.read(buffer, length);
    DWORD dwBytesWritten;
    if (!WriteFile(hDrive, buffer, length, &dwBytesWritten, NULL)) {
        cerr << "Failed to write bytes to physical drive." << endl;
        delete[] buffer;
        CloseHandle(hDrive);
        sourceFile.close();
        return 1;
    }

    // Close the files and release memory
    delete[] buffer;
    CloseHandle(hDrive);
    sourceFile.close();

    return 0;
}
