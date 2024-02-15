/**
 * "HOTLOAD/HOTLOAD.C"
 *
 * (cannot be named "HOT PATCH" because it triggers "UAC")
 */

#include "HOTLOAD.H"

/**************************************************************/

WCHAR const *
wcsrchr(
    WCHAR const *s,
    WCHAR c)
{
    int const length = wcslen(s);

    for (int i = (length - 1); i >= 0; i--)
    {
        if (s[i] == c) { return &(s[i]); }
    }

    return NULL;
}

/**************************************************************/

#pragma function(memcpy)
void *
memcpy(
    void *dest,
    void const *src,
    int n)
{
    char *destBytes = (char *) dest;
    char const *srcBytes = (char const *) src;

    while (n--)
    {
        *(destBytes++) = *(srcBytes++);
    }

    return dest;
}

/**************************************************************/

void *
AllocateMemory(
    void *old_ptr,
    size_t old_size,
    size_t new_size)
{
    void *new_ptr = LocalAlloc(0, new_size);

    if (NULL != old_ptr)
    {
        memcpy(new_ptr, old_ptr, old_size);

        LocalFree(old_ptr);
    }

    return new_ptr;
}

/**************************************************************/

__declspec(noinline)
void
PrintMessageW(
    WCHAR const *message)
{
    uintptr_t const stdout = GetStdHandle(STD_OUTPUT_HANDLE);

    WriteConsoleW(stdout, message, wcslen(message), NULL, NULL);
}

/**************************************************************/

void
Payload_init(
    Payload *self)
{
    self->block = NULL;
    self->blockLen = 0;
    self->absAddr = 0;

    self->numVars = 0;
    self->vars = NULL;
}

/**************************************************************/

void
Payload_destroy(
    Payload *self)
{
    if (NULL == self)
    {
        return;
    }

    if (NULL != (self->block))
    {
        LocalFree(self->block);
    }

    if (NULL != (self->vars))
    {
        LocalFree(self->vars);
    }
}

/**************************************************************/

Payload *
Payload_new(void)
{
    Payload *self = AllocateMemory(
        NULL,
        0,
        sizeof(Payload));

    if (NULL == self)
    {
        return NULL;
    }

    Payload_init(self);

    return self;
}

/**************************************************************/

int
Payload_appendBlockBytes(
    Payload *self,
    uint32_t n,
    size_t nPadded,
    uint8_t const *bytes)
{
    size_t const newBlockLen = self->blockLen + nPadded;

    uint8_t *newBlock = AllocateMemory(
        self->block,
        self->blockLen,
        newBlockLen);

    if (NULL == newBlock)
    {
        return 1;
    }

    memcpy(&(newBlock[self->blockLen]), bytes, n);

    self->block = newBlock;
    self->blockLen = newBlockLen;

    return 0;
}

/**************************************************************/

int
Payload_appendVar(
    Payload *self,
    uint32_t n,
    uint32_t nPadded,
    uint8_t const *bytes)
{
    PayloadVar *result;

    uint32_t const lastVarId = self->numVars;
    uint32_t const newNumVars = lastVarId + 1;
    uint32_t const oldLength = sizeof(PayloadVar) * lastVarId;
    uint32_t const newLength = sizeof(PayloadVar) * newNumVars;

    PayloadVar *newVars = AllocateMemory(
        self->vars,
        oldLength,
        newLength);

    if (NULL == newVars)
    {
        return (-1);
    }

    if (0 != Payload_appendBlockBytes(self, n, nPadded, bytes))
    {
        return (-1);
    }

    result = &(newVars[lastVarId]);

    if (0 == lastVarId)
    {
        result->offset = 0;
    }
    else
    {
        result->offset = result[-1].offset + result[-1].length;
    }

    result->length = nPadded;

    self->vars = newVars;
    self->numVars = newNumVars;

    return lastVarId;
}

/**************************************************************/

int
Payload_appendVarFromCStr(
    Payload *self,
    char const *str)
{
    uint32_t const n1 = 1 + strlen(str);
    uint32_t const n2 = ROUND_UP_TO_4(n1);

    return Payload_appendVar(self, n1, n2, (void const *) str);
}

/**************************************************************/

int
Payload_appendVarFromWStr(
    Payload *self,
    uint16_t const *wcs)
{
    uint32_t const n1 = 2 * (1 + wcslen(wcs));
    uint32_t const n2 = ROUND_UP_TO_4(n1);

    return Payload_appendVar(self, n1, n2, (void const *) wcs);
}

/**************************************************************/

void
Payload_writeAbsVarAddr(
    Payload *self,
    int iVar,
    uint32_t relWriteDest)
{
    /* Storing Litte Endian Asbolute Address of some data */

    PayloadVar const *var = &(self->vars[iVar]);

    uint32_t dummy = (self->absAddr) + (var->offset);
    *((uint32_t *) &(self->block[relWriteDest])) = dummy;
}

/**************************************************************/

void
Payload_writeRelJmpAddr(
    Payload *self,
    uint32_t absTarget,
    uint32_t relNextInstr)
{
    /* Storing Little Endian Relative Address for jumps */

    uint32_t dummy = absTarget - ((self->absAddr) + relNextInstr);
    *((uint32_t *) &(self->block[relNextInstr - 4])) = dummy;
}

/**************************************************************/

void
Payload_writeUInt32(
    Payload *self,
    uint32_t value,
    uint32_t relWriteDest)
{
    /* Storing Little Endian DWORD Constant */

    *((uint32_t *) &(self->block[relWriteDest])) = value;
}

/**************************************************************/

void
Payload_writeUInt8(
    Payload *self,
    uint8_t value,
    uint32_t relWriteDest)
{
    /* Storing BYTE Constant */

    self->block[relWriteDest] = value;
}

/**************************************************************/

int
HotPatch_preparePayload(
    uintptr_t hProcess,
    WCHAR const *modsDir)
{
    BOOL bTest;
    uintptr_t pTest;
    DWORD dwTest;

    Payload payload;
    uintptr_t lpPayloadBase;

    int iVar_WStr_ModsDir;
    int iVar_CStr_Kao2PlusDll;

    uintptr_t hPayloadThread;
    uintptr_t hKernel32;

    uintptr_t pfn_GetModuleHandleA;
    uintptr_t pfn_FreeLibrary;
    uintptr_t pfn_SetDllDirectoryW;
    uintptr_t pfn_LoadLibraryA;

    /* Get Win32 procedure addresses */

    PrintMessageW(L"* Loading procedure addresses...\n");

    hKernel32 = GetModuleHandleA("KERNEL32.DLL");

    if (0 == hKernel32)
    {
        PrintMessageW(L"* ERROR: Could not load \"KERNEL32.DLL\".\n");
        return 1;
    }

    pfn_GetModuleHandleA = GetProcAddress(hKernel32, "GetModuleHandleA");

    if (0 == pfn_GetModuleHandleA)
    {
        PrintMessageW(L"* ERROR: Could not find \"GetModuleHandleA\".\n");
        return 1;
    }

    pfn_FreeLibrary = GetProcAddress(hKernel32, "FreeLibrary");

    if (0 == pfn_FreeLibrary)
    {
        PrintMessageW(L"* ERROR: Could not find \"FreeLibrary\".\n");
        return 1;
    }

    pfn_SetDllDirectoryW = GetProcAddress(hKernel32, "SetDllDirectoryW");

    if (0 == pfn_SetDllDirectoryW)
    {
        PrintMessageW(L"* ERROR: Could not find \"SetDllDirectoryW\".\n");
        return 1;
    }

    pfn_LoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");

    if (0 == pfn_LoadLibraryA)
    {
        PrintMessageW(L"* ERROR: Could not find \"LoadLibraryA\".\n");
        return 1;
    }

    /* Prepare payload thread (data to write) */

    PrintMessageW(L"* Building payload bytes...\n");

    Payload_init(&(payload));

    bTest = Payload_appendVar(
        &(payload),
        ARRAY_SIZE_BYTES(g_szLoaderThreadPayload),
        ROUND_UP_TO_16(ARRAY_SIZE_BYTES(g_szLoaderThreadPayload)),
        g_szLoaderThreadPayload);

    if (bTest < 0)
    {
        PrintMessageW(L"* ERROR: Could not add \"szLoaderThreadPayload\".\n");
        Payload_destroy(&(payload));
        return 1;
    }

    iVar_WStr_ModsDir =
        Payload_appendVarFromWStr(&(payload), modsDir);

    if (iVar_WStr_ModsDir < 0)
    {
        PrintMessageW(L"* ERROR: Could not add \"var_WStr_ModsDir\".\n");
        Payload_destroy(&(payload));
        return 1;
    }

    iVar_CStr_Kao2PlusDll =
        Payload_appendVarFromCStr(&(payload), "Kao2Plus.DLL");

    if (iVar_CStr_Kao2PlusDll < 0)
    {
        PrintMessageW(L"* ERROR: Could not add \"var_CStr_Kao2PlusDll\".\n");
        Payload_destroy(&(payload));
        return 1;
    }

    /* Allocate additional block for the game process */

    PrintMessageW(L"* Allocating payload page...\n");

    pTest = VirtualAllocEx(
        hProcess,
        0,
        payload.blockLen,
        MEM_COMMIT,
        PAGE_EXECUTE_READ);

    if (0 == pTest)
    {
        PrintMessageW(L"* ERROR: \"VirtualAllocEx\" failed.\n");
        Payload_destroy(&(payload));
        return 1;
    }

    lpPayloadBase = pTest;
    payload.absAddr = (uint32_t) lpPayloadBase;

    /* Prepare payload thread (fill in "zeros") */

    PrintMessageW(L"* Filling payload details...\n");

    Payload_writeAbsVarAddr(&(payload), iVar_CStr_Kao2PlusDll, 0x00 + 1);
    Payload_writeRelJmpAddr(&(payload), pfn_GetModuleHandleA,  0x0A);
    Payload_writeRelJmpAddr(&(payload), pfn_FreeLibrary,       0x14);

    Payload_writeAbsVarAddr(&(payload), iVar_WStr_ModsDir,     0x1F + 1);
    Payload_writeRelJmpAddr(&(payload), pfn_SetDllDirectoryW,  0x29);
    Payload_writeRelJmpAddr(&(payload), pfn_SetDllDirectoryW,  0x4E);

    Payload_writeAbsVarAddr(&(payload), iVar_CStr_Kao2PlusDll, 0x32 + 1);
    Payload_writeRelJmpAddr(&(payload), pfn_LoadLibraryA,      0x3C);

    /* Store the complete payload into game memory */

    PrintMessageW(L"* Storing the final payload...\n");

    bTest = WriteProcessMemory(
        hProcess,
        lpPayloadBase,
        (void const *) (payload.block),
        payload.blockLen,
        NULL);

    Payload_destroy(&(payload));

    if (FALSE == bTest)
    {
        PrintMessageW(L"* ERROR: \"WriteProcessMemory\" failed.\n");
        return 1;
    }

    /**
     * Start a custom payload thread.
     *
     * Once any game thread is resumed, all dependencies
     * ("KERNELBASE", "KERNEL32", "GLUT32", "OPENGL32", ...)
     * are properly loaded before the code is executed.
     */
    PrintMessageW(L"* Launching payload thread...\n");

    hPayloadThread = CreateRemoteThread(
        hProcess,
        0,
        0,
        lpPayloadBase,
        NULL,
        0,
        NULL);

    if (0 == hPayloadThread)
    {
        PrintMessageW(L"* ERROR: Remote Thread failed to launch.\n");
        return 1;
    }

    WaitForSingleObject(hPayloadThread, INFINITE);
    bTest = GetExitCodeThread(hPayloadThread, &(dwTest));
    CloseHandle(hPayloadThread);

    if ((FALSE == bTest) or (0 != dwTest))
    {
        PrintMessageW(L"* ERROR: Thread ErrorLevel ");
        PrintMessageW(g_szLoaderThreadErrors[dwTest]);
        PrintMessageW(L"\n");

        return 1;
    }

    VirtualFreeEx(hProcess, lpPayloadBase, 0, MEM_RELEASE);

    PrintMessageW(L"* \"Kao2Plus\" loaded!\n");

    return 0;
}

/**************************************************************/

uintptr_t
HotPatch_getGameProcess(
    WCHAR const *gameExec)
{
    BOOL bTest;
    DWORD arraySize;

    DWORD const dwDesiredAccess =
        PROCESS_VM_OPERATION |
        PROCESS_VM_READ |
        PROCESS_VM_WRITE |
        PROCESS_QUERY_INFORMATION |
        PROCESS_SUSPEND_RESUME;

    /* Enumerate Win32 processes */

    arraySize = sizeof(DWORD) * MAX_PROCESSES;

    PrintMessageW(L"* Enumerating Win32 processes...\n");

    DWORD *pids = LocalAlloc(0, arraySize);

    if (NULL == pids) { return 0; }

    bTest = K32EnumProcesses(
        pids,
        arraySize,
        &(arraySize));

    if (not bTest)
    {
        LocalFree(pids);
        return 0;
    }

    /* Try opening each process and checking its filename */

    arraySize /= sizeof(DWORD);

    for (size_t i = 0; i < arraySize; i++)
    {
        WCHAR fileName[MAX_PATH];

        uintptr_t hProcess = OpenProcess(
            dwDesiredAccess,
            FALSE,
            pids[i]);

        if (0 == hProcess) { continue; }

        K32GetProcessImageFileNameW(
            hProcess,
            fileName,
            MAX_PATH);

        WCHAR *backslashPos = (WCHAR *) wcsrchr(fileName, '\\');

        if ((NULL != backslashPos) and
            (0 == wcscmp(gameExec, &(backslashPos[1]))))
        {
            /* Game found and accessible! */

            PrintMessageW(L"* Found \"");
            PrintMessageW(gameExec);
            PrintMessageW(L"\" at \"");
            PrintMessageW(fileName);
            PrintMessageW(L"\".\n");

            return hProcess;
        }

        CloseHandle(hProcess);
    }

    /* Process not found. */

    LocalFree(pids);
    return 0;
}

/**************************************************************/

int
HotPatch_areYouKiddingMe(
    WCHAR const *modsDir,
    WCHAR const *gameExec)
{
    uintptr_t hProcess = HotPatch_getGameProcess(gameExec);

    if (0 == hProcess)
    {
        PrintMessageW(L"* ERROR: Game Process not found.\n");
        return 1;
    }

    PrintMessageW(L"\n");

    int iRet = HotPatch_preparePayload(
        hProcess,
        modsDir);

    CloseHandle(hProcess);

    return iRet;
}

/**************************************************************/

int
HotPatch_processCmdLine(
    WCHAR *modsDir,
    WCHAR *gameExec)
{
    int argc;
    WCHAR *backslashPos;

    WCHAR *commandLine = GetCommandLineW();
    WCHAR **argv = CommandLineToArgvW(commandLine, &(argc));

    /* Extract game executable name */

    if (argc < 2)
    {
        wcscpy(gameExec, L"kao2.exe");
    }
    else
    {
        wcscpy(gameExec, argv[1]);
    }

    /* Extract "mods" directory from `argv[0]` */

    backslashPos = (WCHAR *) wcsrchr(argv[0], L'\\');

    if (NULL == backslashPos)
    {
        modsDir[0] = L'\0';
        GetCurrentDirectoryW(MAX_PATH, modsDir);
    }
    else
    {
        backslashPos[0] = L'\0';

        if (PathIsRelativeW(argv[0]))
        {
            modsDir[0] = L'\0';
            DWORD length = GetCurrentDirectoryW(MAX_PATH, modsDir);

            modsDir[length] = L'\\';
            wcscpy(&(modsDir[length + 1]), argv[0]);
        }
        else
        {
            wcscpy(modsDir, argv[0]);
        }
    }

    LocalFree(argv);
    return 0;
}

/**************************************************************/

void
__stdcall
EntryPoint()
{
    int iRet;

    WCHAR wchModsDir[MAX_PATH];
    WCHAR wchGameExec[32];

    PrintMessageW(L"********************************\n");
    PrintMessageW(L"* HotLoad (Kao2Plus)           *\n");
    PrintMessageW(L"********************************\n");

    PrintMessageW(L"\n");

    iRet = HotPatch_processCmdLine(
        wchModsDir,
        wchGameExec);

    if (0 != iRet) { ExitProcess(1); }

    PrintMessageW(L"* modsDir = \"");
    PrintMessageW(wchModsDir);
    PrintMessageW(L"\"\n");

    PrintMessageW(L"* gameExec = \"");
    PrintMessageW(wchGameExec);
    PrintMessageW(L"\"\n");

    PrintMessageW(L"\n");

    iRet = HotPatch_areYouKiddingMe(
        wchModsDir,
        wchGameExec);

    ExitProcess(iRet);
}

/**************************************************************/
