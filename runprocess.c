#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

int main() {
    HANDLE hSnapshot;
    PROCESSENTRY32 entry;

    // Take a snapshot of all processes in the system
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Failed to create process snapshot. Error: %lu\n", GetLastError());
        return 1;
    }

    // Initialize the PROCESSENTRY32 structure
    entry.dwSize = sizeof(PROCESSENTRY32);

    // Start enumerating processes
    if (Process32First(hSnapshot, &entry)) {
        do {
            // Print the process name and PID
            _tprintf(_T(" %s\t %lu\n"), entry.szExeFile, entry.th32ProcessID);
        } while (Process32Next(hSnapshot, &entry)); // Continue to the next process
    } else {
        printf("Failed to enumerate processes. Error: %lu\n", GetLastError());
    }

    // Clean up the snapshot object
    CloseHandle(hSnapshot);
    return 0;
}
