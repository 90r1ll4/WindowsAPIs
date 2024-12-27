#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <tchar.h>

int main(int argc, const char* argv[]) {
    if (argc < 3){
        printf("Usage: %s <pid> <dllpath>", argv[0]);
        return 0;
    }
    auto pid =  atoi(argv[1]);
    const char *message = "Message from injected thread!";

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        printf("Failed to open process. Error: (%u)", GetLastError());
        return 1;
    }

    // Step 2: Allocate memory in the remote process
    auto remoteMemory = VirtualAllocEx(hProcess, NULL, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    // if (!remoteMemory) {
    //     cout << "Failed to allocate memory in target process. Error: " << GetLastError() << endl;
    //     CloseHandle(hProcess);
    //     return 1;
    // }
        assert(remoteMemory);
    printf(" allocated at address %p\n", remoteMemory);
    // Step 3: Write the payload into the allocated memory
    WriteProcessMemory(hProcess, remoteMemory, argv[2], strlen(argv[2]) + 1, NULL);
    // if (!writeStatus) {
    //     cout << "Failed to write to process memory. Error: " << GetLastError() << endl;
    //     VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
    //     CloseHandle(hProcess);
    //     return 1;
    // }

    // Step 4: Create a remote thread to execute MessageBoxA in the target process
    // auto hThread = CreateRemoteThread(
    //     hProcess,                // Handle to the target process
    //     NULL,                    // Default security attributes
    //     0,                       // Stack size (0 = default)
    //     (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"LoadLibraryA"), // Entry point (LoadLibraryA in this case)
    //     remoteMemory,            // Argument (address of the message string)
    //     0,                       // Creation flags (0 = run immediately)
    //     NULL                     // Return thread ID (optional)
    // );
        auto hThread = CreateRemoteThread(hProcess,nullptr, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32"), "LoadLibraryW"),
        remoteMemory, 0, nullptr);
        printf("Thread handle: %lu\n", (uintptr_t)hThread);
        printf("Error code: %lu\n", GetLastError());

        assert(hThread);
        printf(" Thread %p\n", hThread);
    // if (!hThread) {
    //     cout << "Failed to create remote thread. Error: " << GetLastError() << endl;
    //     VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
    //     CloseHandle(hProcess);
    //     return 1;
    // }

    printf("Successfully injected a thread into PID %s", pid);

    // Wait for the thread to complete execution
    WaitForSingleObject(hThread, INFINITE);

    // // Cleanup resources
    VirtualFreeEx(hProcess, remoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return 0;
}
