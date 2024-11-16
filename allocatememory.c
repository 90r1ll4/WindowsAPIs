#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 4096
#define MAX_MEMORY 10000

void print_permission(DWORD protection) {
    switch (protection) {
        case PAGE_READONLY:
            printf("READ_ONLY\n");
            break;
        case PAGE_EXECUTE:
            printf("EXECUTE\n");
            break;
        case PAGE_READWRITE:
            printf("READ_WRITE\n");
            break;
        case PAGE_EXECUTE_READWRITE:
            printf("READ_WRITE_EXECUTE\n");
            break;
        case PAGE_EXECUTE_READ:
            printf("READ_EXECUTE\n");
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }
}

int main() {
    DWORD pid;
    HANDLE hProcess;
    SIZE_T alloc_size;
    LPVOID remote_mem;
    DWORD protection;
    int perm_choice;

    printf("Welcome\n==========\n");
    printf("To run this sample, you will need a PID of a dummy process\n");
    printf("to allocate memory into. Open Notepad and use Process Hacker to obtain its PID.\n");
    printf("==========\n");

    printf("Enter PID of Notepad.exe: ");
    scanf("%lu", &pid);

    printf("You have requested us to allocate memory into PID: %lu\n-------------\n", pid);

    // Open the target process with appropriate permissions
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        printf("Error: Unable to open process. Code: %lu\n", GetLastError());
        return 1;
    }

    // Memory allocation input
    do {
        printf(">> Enter amount of memory to allocate. Maximum %d: ", MAX_MEMORY);
        scanf("%zu", &alloc_size);

        if (alloc_size > MAX_MEMORY) {
            printf("XXXXXX Invalid value\n");
        }
    } while (alloc_size > MAX_MEMORY);

    // Round up to the nearest page size
    alloc_size = ((alloc_size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
    printf("Allocating %zu bytes after rounding up to page size of %d bytes\n", alloc_size, PAGE_SIZE);

    // Memory permissions input
    do {
        printf(">> Enter memory permissions\n");
        printf(">> Enter 1: R\n>> Enter 2: X\n>> Enter 3: RW\n>> Enter 4: RWX\n>> Enter 5: RX\n");
        printf("Enter your value(1-5): ");
        scanf("%d", &perm_choice);

        switch (perm_choice) {
            case 1: protection = PAGE_READONLY; break;
            case 2: protection = PAGE_EXECUTE; break;
            case 3: protection = PAGE_READWRITE; break;
            case 4: protection = PAGE_EXECUTE_READWRITE; break;
            case 5: protection = PAGE_EXECUTE_READ; break;
            default: printf("Invalid choice. Try again.\n"); protection = 0;
        }
    } while (!protection);

    printf("You chose ");
    print_permission(protection);

    printf("=============Allocating memory in remote target process...\n");

    // Allocate memory in the target process
    remote_mem = VirtualAllocEx(hProcess, NULL, alloc_size, MEM_COMMIT | MEM_RESERVE, protection);
    if (!remote_mem) {
        printf("Error: Memory allocation failed. Code: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return 1;
    }

    printf(">> Memory of size %zu allocated at address %p\n", alloc_size, remote_mem);
    printf(">> Please verify using Process Hacker if memory of size %zu is allocated at address %p inside Notepad.exe process with the permissions you specified\n",
           alloc_size, remote_mem);

    // Clean up
    CloseHandle(hProcess);
    return 0;
}
