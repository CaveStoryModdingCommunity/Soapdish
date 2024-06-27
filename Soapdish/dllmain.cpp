#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

bool applyPatches();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    try
    {
        if (ul_reason_for_call == DLL_PROCESS_ATTACH)
            return applyPatches();
    }
    catch (...)
    {
        return FALSE;
    }
    return TRUE;
}

