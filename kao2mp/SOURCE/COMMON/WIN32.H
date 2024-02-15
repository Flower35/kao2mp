/**
 * "COMMON/WIN32.H"
 */

/**************************************************************/
/* Windows defines                                            */
/**************************************************************/

#define FALSE  0
#define TRUE   (!0)

#define __TEXT(quote)  L##quote
#define TEXT(quote)    __TEXT(quote)

#define MAX_PATH  260

#define PROCESS_VM_OPERATION       0x0008
#define PROCESS_VM_READ            0x0010
#define PROCESS_VM_WRITE           0x0020
#define PROCESS_QUERY_INFORMATION  0x0400
#define PROCESS_SUSPEND_RESUME     0x0800

#define MEM_COMMIT   0x00001000
#define MEM_RELEASE  0x00008000

#define PAGE_EXECUTE_READWRITE  0x40
#define PAGE_EXECUTE_READ       0x20

#define DLL_PROCESS_DETACH  0
#define DLL_PROCESS_ATTACH  1

#define STD_OUTPUT_HANDLE  ((DWORD) (-11))

#define INFINITE  0xFFFFFFFF

#define MB_ICONWARNING      0x30
#define MB_ICONINFORMATION  0x40

#define INVALID_HANDLE_VALUE  ((uintptr_t)(-1))

#define INVALID_FILE_SIZE ((DWORD) 0xFFFFFFFF)

#define GENERIC_READ  (0x80000000L)

#define OPEN_EXISTING  3

/**************************************************************/
/* DirectInput defines                                        */
/**************************************************************/

#define DIK_ESCAPE    0x01
#define DIK_LBRACKET  0x1A
#define DIK_RBRACKET  0x1B
#define DIK_RETURN    0x1C
#define DIK_UP        0xC8
#define DIK_LEFT      0xCB
#define DIK_RIGHT     0xCD
#define DIK_DOWN      0xD0

/**************************************************************/
/* Windows types                                              */
/**************************************************************/

typedef int      BOOL;
typedef uint8_t  BYTE;
typedef uint16_t WCHAR;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef int64_t  LONGLONG;

typedef union _LARGE_INTEGER LARGE_INTEGER;

union _LARGE_INTEGER
{
  struct { DWORD LowPart; LONG HighPart; } DUMMYSTRUCTNAME;
  struct { DWORD LowPart; LONG HighPart; } u;
  LONGLONG QuadPart;
};

/**************************************************************/
/* "KERNEL32.LIB" : Process Threads API                       */
/**************************************************************/

__declspec(noreturn)
void
__stdcall
ExitProcess(DWORD);

uintptr_t
__stdcall
CreateRemoteThread(uintptr_t, uintptr_t, size_t, uintptr_t, void *, DWORD, DWORD *);

uintptr_t
__stdcall
CreateThread(uintptr_t, size_t, void *, uintptr_t, DWORD, DWORD *);

BOOL
__stdcall
TerminateThread(uintptr_t, DWORD);

BOOL
__stdcall
GetExitCodeThread(uintptr_t, DWORD *);

uintptr_t
__stdcall
OpenProcess(DWORD, BOOL, DWORD);

/**************************************************************/
/* "KERNEL32.LIB" : Process Status API                        */
/**************************************************************/

BOOL
__stdcall
K32EnumProcesses(DWORD *, DWORD, DWORD *);

DWORD
__stdcall
K32GetProcessImageFileNameW(uintptr_t, WCHAR *, DWORD);

/**************************************************************/
/* "KERNEL32.LIB" : Handle API                                */
/**************************************************************/

BOOL
__stdcall
CloseHandle(uintptr_t);

/**************************************************************/
/* "KERNEL32.LIB" : Synch API                                 */
/**************************************************************/

DWORD
__stdcall
WaitForSingleObject(uintptr_t, DWORD);

/**************************************************************/
/* "KERNEL32.LIB" : Profile API                               */
/**************************************************************/

BOOL
__stdcall
QueryPerformanceFrequency(LARGE_INTEGER *);

BOOL
__stdcall
QueryPerformanceCounter(LARGE_INTEGER *);

/**************************************************************/
/* "KERNEL32.LIB" : System Info API                           */
/**************************************************************/

DWORD
__stdcall
GetTickCount(void);

/**************************************************************/
/* "KERNEL32.LIB" : LibLoader API                             */
/**************************************************************/

uintptr_t
__stdcall
GetStdHandle(DWORD);

uintptr_t
__stdcall
GetModuleHandleA(char const *);

uintptr_t
__stdcall
GetProcAddress(uintptr_t, char const *);

/**************************************************************/
/* "KERNEL32.LIB" : Console API                               */
/**************************************************************/

BOOL
__stdcall
WriteConsoleA(uintptr_t, char const *, size_t, DWORD *, void *);

BOOL
__stdcall
WriteConsoleW(uintptr_t, WCHAR const *, size_t, DWORD *, void *);

/**************************************************************/
/* "KERNEL32.LIB" : File API                                  */
/**************************************************************/

uintptr_t
__stdcall
CreateFileA(char const *, DWORD, DWORD, uintptr_t, DWORD, DWORD, uintptr_t);

uintptr_t
__stdcall
CreateFileW(WCHAR const *, DWORD, DWORD, uintptr_t, DWORD, DWORD, uintptr_t);

BOOL
__stdcall
ReadFile(uintptr_t, void *, DWORD, DWORD *, uintptr_t);

DWORD
__stdcall
GetFileSize(uintptr_t, DWORD *);

/**************************************************************/
/* "KERNEL32.LIB" : Process Env                               */
/**************************************************************/

WCHAR *
__stdcall
GetCommandLineW(void);

/**************************************************************/
/* "KERNEL32.LIB" : WinBase                                   */
/**************************************************************/

DWORD
__stdcall
GetCurrentDirectoryW(DWORD, WCHAR *);

DWORD
__stdcall
GetDllDirectoryW(DWORD, WCHAR *);

__declspec(allocator)
void *
__stdcall
LocalAlloc(DWORD, size_t);

void *
__stdcall
LocalFree(void *);

/**************************************************************/
/* "KERNEL32.LIB" : Memory API                                */
/**************************************************************/

uintptr_t
__stdcall
VirtualAllocEx(uintptr_t, uintptr_t, size_t, DWORD, DWORD);

BOOL
__stdcall
VirtualFreeEx(uintptr_t, uintptr_t, size_t, DWORD);

BOOL
__stdcall
VirtualProtect(uintptr_t, size_t, DWORD, DWORD *);

BOOL
__stdcall
WriteProcessMemory(uintptr_t, uintptr_t, void const *, size_t, size_t *);

/**************************************************************/
/* "USER32.LIB"                                               */
/**************************************************************/

int
__stdcall
MessageBoxA(uintptr_t, char const *, char const *, unsigned);

/**************************************************************/
/* "SHELL32.LIB"                                              */
/**************************************************************/

WCHAR **
__stdcall
CommandLineToArgvW(WCHAR const *, int *);

/**************************************************************/
/* "SHLWAPI.LIB"                                              */
/**************************************************************/

BOOL
__stdcall
PathIsRelativeW(WCHAR const *);

/**************************************************************/
