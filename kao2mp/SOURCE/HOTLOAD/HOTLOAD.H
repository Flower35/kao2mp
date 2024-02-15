/**
 * "HOTLOAD/HOTLOAD.H"
 */

/**************************************************************/

#include <iso646.h>
#include <stdint.h>

#include "COMMON/WIN32.H"

/**************************************************************/

#define MAX_PROCESSES  1024

#define ARRAY_SIZE_BYTES(array) (sizeof(array) / sizeof(*array))

#define ROUND_UP_TO_4(value) (((value) + 3) & ~3)

#define ROUND_UP_TO_16(value) (((value) + 15) & ~15)

/**************************************************************/

typedef struct PayloadVar PayloadVar;

struct PayloadVar
{
    uint32_t offset;
    size_t length;
};

/**************************************************************/

typedef struct Payload Payload;

struct Payload
{
    uint8_t *block;
    size_t blockLen;
    uint32_t absAddr;
    int32_t numVars;
    PayloadVar *vars;
};

/**************************************************************/

WCHAR const *const g_szLoaderThreadErrors[] =
{
    L"[0] Success",
    L"[1] SetDllDirectoryW() failed",
    L"[2] LoadLibraryA(Kao2Plus) failed",
    L"[3] FreeLibrary(Kao2Plus) failed",
};

/**************************************************************/

uint8_t const g_szLoaderThreadPayload[] =
{
    0x68, 0x00, 0x00, 0x00, 0x00,  // [$+00] push <var_CStr_Kao2PlusDll>
    0xE8, 0x00, 0x00, 0x00, 0x00,  // [$+05] call <GetModuleHandleA>
    0x85, 0xC0,                    // [$+0A]     test eax,eax
    0x74, 0x11,                    // [$+0C]     jz short ($+1F)
    0x50,                          // [$+0E] push eax
    0xE8, 0x00, 0x00, 0x00, 0x00,  // [$+0F] call <FreeLibrary>
    0x85, 0xC0,                    // [$+14]     test eax,eax
    0x75, 0x07,                    // [$+16]     jnz short ($+1F)
    0xB8, 0x03, 0x00, 0x00, 0x00,  // [$+18]     mov eax,3
    0xEB, 0x34,                    // [$+1D]     jmp short ($+53)
    0x68, 0x00, 0x00, 0x00, 0x00,  // [$+1F] push <var_WStr_ModsDir>
    0xE8, 0x00, 0x00, 0x00, 0x00,  // [$+24] call <SetDllDirectoryW>
    0x85, 0xC0,                    // [$+29]     test eax,eax
    0x75, 0x05,                    // [$+2B]     jnz short ($+32)
    0x31, 0xC0,                    // [$+2D]     xor eax,eax
    0x40,                          // [$+2F]     inc eax
    0xEB, 0x21,                    // [$+30]     jmp short ($+53)
    0x68, 0x00, 0x00, 0x00, 0x00,  // [$+32] push <var_CStr_Kao2PlusDll>
    0xE8, 0x00, 0x00, 0x00, 0x00,  // [$+37] call <LoadLibraryA>
    0x85, 0xC0,                    // [$+3C]     test eax,eax
    0x75, 0x07,                    // [$+3E]     jnz short ($+47)
    0xB8, 0x02, 0x00, 0x00, 0x00,  // [$+40]     mov eax,2
    0xEB, 0x0C,                    // [$+45]     jmp short ($+53)
    0x6A, 0x00,                    // [$+47] push 0
    0xE8, 0x00, 0x00, 0x00, 0x00,  // [$+49] call <SetDllDirectoryW>
    0xF7, 0xD8,                    // [$+4E]     neg eax
    0x19, 0xC0,                    // [$+50]     sbb eax,eax
    0x40,                          // [$+52]     inc eax
    0xC2, 0x04, 0x00,              // [$+53] ret 0x4
                                   // [$+56]
};

/**************************************************************/
