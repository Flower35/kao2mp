/**
 * "SERVER/SERVER2.C"
 */

#include "SERVER.H"

/**************************************************************/

unsigned _fltused;

/**************************************************************/

#pragma function(memcmp)
int
memcmp(
    void const *mem1,
    void const *mem2,
    size_t n)
{
    char const *s1 = (char const *) mem1;
    char const *s2 = (char const *) mem2;

    for (; n--; s1++, s2++)
    {
        if ((*s1) != (*s2)) { return (*s1) - (*s2); }
    }

    return 0;
}

/**************************************************************/

#pragma function(memcpy)
void *
memcpy(
    void *dest,
    void const *src,
    size_t n)
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

char *
__cdecl
_strdup(
    char const *str)
{
    size_t const len = strlen(str);
    char *result = mallocWrapper(1 + len);

    memcpy(result, str, len);
    result[len] = '\0';

    return result;
}

/**************************************************************/

float
__cdecl
_lltof(
    int64_t x)
{
    __asm
    {
        fild    x
    }
}

/**************************************************************/

void *
mallocWrapper(
    size_t size)
{
    return LocalAlloc(0, size);
}

/**************************************************************/

void
freeWrapper(
    uintptr_t mem)
{
    LocalFree((void *) mem);
}

/**************************************************************/

__declspec(noinline)
void
PrintMessageA(
    char const *message)
{
    uintptr_t const stdout = GetStdHandle(STD_OUTPUT_HANDLE);

    WriteConsoleA(stdout, message, strlen(message), NULL, NULL);
}

/**************************************************************/

uint32_t
kao2mp_Server_CalcChksum(
    void const *data,
    size_t size)
{
    uint32_t const *dwords = (uint32_t const *) data;

    size /= sizeof(uint32_t);

    uint32_t sum = 0;

    for (size_t i = 0; i < size; i++) { sum += dwords[i]; }

    return sum;
}

/**************************************************************/

uint32_t
kao2mp_Server_CalcNewUId(void)
{
    /**
     * This is just for assertion that
     * no client impersonates another player.
     *
     * Should in fact just check for IP addresses I think.
     */

    uint32_t const randomSecret = 0x5014c471;

    return GetTickCount() ^ randomSecret;
}

/**************************************************************/

int32_t
kao2mp_Server_GetNumActivePlayers(void)
{
    int result = 0;

    for (int i = 0; i < gPlayersCount; i++)
    {
        if (0 != gPlayers[i].uid) { result++; }
    }

    return result;
}

/**************************************************************/

int32_t
kao2mp_Server_GetFirstFreeIndex(void)
{
    for (int i = 0; i < gPlayersCount; i++)
    {
        if (0 == gPlayers[i].uid) { return i; }
    }

    return gPlayersCount;
}

/**************************************************************/

__declspec(noinline)
void
kao2mp_Server_ResetPlayer(
    int const index)
{
    gPlayers[index].timeOut = (-1);
    gPlayers[index].uid = 0;

    gPlayers[index].data.data.active = false;

    /* Other data will be garbage XD */

    gPlayers[index].data.chksum = kao2mp_Server_CalcChksum(
        &(gPlayers[index].data.data),
        sizeof(gPlayers[index].data.data));
}

/**************************************************************/

bool
kao2mp_Server_SetPlayersArraySize(
    int newPlayerCount)
{
    size_t const allocSize = newPlayerCount * sizeof(kao2_PlayerServerData);

    if (NULL == gPlayers)
    {
        gPlayers = (kao2_PlayerServerData *) mallocWrapper(allocSize);

        gPlayersAlloc = newPlayerCount;
    }
    else if (newPlayerCount > gPlayersAlloc)
    {
        kao2_PlayerServerData *newPlayers =
            (kao2_PlayerServerData *) mallocWrapper(allocSize);

        if (NULL == newPlayers) { return false; }

        for (int i = 0; i < gPlayersCount; i++)
        {
            newPlayers[i] = gPlayers[i];
        }

        freeWrapper((uintptr_t) gPlayers);

        gPlayers = newPlayers;

        gPlayersAlloc = newPlayerCount;
    }

    for (int i = gPlayersCount; i < newPlayerCount; i++)
    {
        kao2mp_Server_ResetPlayer(i);
    }

    gPlayersCount = newPlayerCount;

    return true;
}

/**************************************************************/

void
Dummy_UInt16ToTextDec(
    char *buf,
    uint16_t value)
{
    /* (2^16 - 1) == 65535 */

    buf[0] = '0';
    buf[1] = '0';
    buf[2] = '0';
    buf[3] = '0';
    buf[4] = '0';

    buf += 5;

    buf[0] = '\0';

    buf--;

    while (0 != value)
    {
        int digit = value % 10;
        value = value / 10;

        *buf = (char) ('0' + digit);
        buf--;
    }
}

/**************************************************************/

void
Dummy_UInt32ToText(
    char *buf,
    uintptr_t value)
{
    buf[0] = '0';
    buf[1] = 'x';

    buf += 2;

    buf[0] = '0';
    buf[1] = '0';
    buf[2] = '0';
    buf[3] = '0';
    buf[4] = '0';
    buf[5] = '0';
    buf[6] = '0';
    buf[7] = '0';

    buf += 8;

    buf[0] = '\0';

    buf--;

    while (0 != value)
    {
        int digit = value % 16;
        value = value / 16;

        if ((digit >= 0) and (digit <= 9))
        {
            *buf = (char) ('0' + digit);
        }
        else
        {
            *buf = (char) ('a' + (digit - 10));
        }

        buf--;
    }
}

/**************************************************************/

int32_t
Dummy_TextToInt32(
    char const *buf)
{
    bool sign = false;
    int32_t value = 0;

    if ((*buf) and ('-' == (*buf)))
    {
        sign = true;
        buf++;
    }

    while (*buf)
    {
        if (((*buf) >= '0') and ((*buf) <= '9'))
        {
            value = 10 * value + ((*buf) - '0');
        }
        else
        {
            return 0;
        }

        buf++;
    }

    return sign ? (- value) : value;
}

/**************************************************************/

void
kao2mp_Server_PrintAddressInfo(
    SOCKADDR_IN const *sockAddrIn)
{
    char addrBuf[INET_ADDRSTRLEN];
    char numericBuf[5 + 1];

    int addrBufSize = INET_ADDRSTRLEN;

    /* Socket Address as a text */

    addrBuf[0] = '\0';

    inet_ntop(
        AF_INET,
        &(sockAddrIn->sin_addr),
        addrBuf,
        &(addrBufSize));

    /* Socket Port as a text */

    Dummy_UInt16ToTextDec(
        numericBuf,
        ntohs(sockAddrIn->sin_port));

    /* Print info */

    PrintMessageA("[");
    PrintMessageA(addrBuf);
    PrintMessageA("]:[");
    PrintMessageA(numericBuf);
    PrintMessageA("]");
}

/**************************************************************/

bool
kao2mp_Server_ParseConfig(
    BYTE    *fileData,
    int32_t fileSize,
    char    **serverIP,
    char    **serverPort,
    char    **serverPass)
{
    /**
     * [server]
     *   ip = 0.0.0.0
     *   port = 65535
     *   pass = 123
     */

    bool isServerTag = false;

    /**
     * Actual allocated size is (File Size + 1).
     * Place a line-break guard at the very end.
     */
    fileData[fileSize] = '\n';

    /* Replace "CR LF" with "LF" */

    for (int i = 0; i < (fileSize - 1); i++)
    {
        if ('\r' == fileData[i])
        {
            if ('\n' != fileData[i + 1])
            {
                /* Bad ".INI" file */

                return false;
            }

            for (int ii = i; ii < (fileSize - 1 + 1); ii++)
            {
                fileData[ii] = fileData[ii + 1];
            }

            fileSize--;
        }
    }

    /* Process each line */

    int lineStart = 0;
    int lineEnd;

    for (int i = 0; i < fileSize; i++)
    {
        if ('\n' == fileData[i])
        {
            /* Look for an ";" comment */

            lineEnd = i;

            for (int ii = lineStart; ii < i; ii++)
            {
                if (';' == fileData[ii])
                {
                    lineEnd = ii;
                    break;
                }
            }

            /* Skip whitespace from the left side */

            while ((lineStart < lineEnd) and
                ((' ' == fileData[lineStart]) or
                ('\t' == fileData[lineStart])))
            {
                lineStart++;
            }

            /* Skip whitespace from the right side */

            while ((lineStart < lineEnd) and
                ((' ' == fileData[lineEnd - 1]) or
                ('\t' == fileData[lineEnd - 1])))
            {
                lineEnd--;
            }

            if (lineStart < lineEnd)
            {
                /* Check if a "[tag]" is reached */

                if ('[' == fileData[lineStart])
                {
                    if (']' != fileData[lineEnd - 1])
                    {
                        /* Bad ".INI" file */

                        return false;
                    }

                    fileData[lineEnd - 1] = '\0';

                    isServerTag = false;

                    char const *const tagName =
                        (char const *) &(fileData[lineStart + 1]);

                    if (0 == strcmp(tagName, "server"))
                    {
                        isServerTag = true;
                    }
                    else
                    {
                        /* Unrecognized tag */

                        return false;
                    }
                }
                else
                {
                    /* Parse "key = value" line */

                    if (not isServerTag)
                    {
                        /* Bad ".INI" file */

                        return false;
                    }

                    int splitPos = (-1);

                    for (int ii = lineStart; ii < lineEnd; ii++)
                    {
                        if ('=' == fileData[ii])
                        {
                            splitPos = ii;
                            break;
                        }
                    }

                    if (splitPos < 0)
                    {
                        /* Bad ".INI" file */

                        return false;
                    }

                    /* Trim and check the value */

                    int valueStart = splitPos + 1;

                    while ((valueStart < lineEnd) and
                        ((' ' == fileData[valueStart]) or
                        ('\t' == fileData[valueStart])))
                    {
                        valueStart++;
                    }

                    if (valueStart >= lineEnd)
                    {
                        /* Bad ".INI" file */

                        return false;
                    }

                    if ('"' == fileData[valueStart])
                    {
                        if ('"' != fileData[lineEnd - 1])
                        {
                            /* Bad ".INI" file */

                            return false;
                        }

                        valueStart++;
                        lineEnd--;
                    }

                    fileData[lineEnd] = '\0';

                    /* Trim and check the key */

                    while ((lineStart < splitPos) and
                        ((' ' == fileData[splitPos - 1]) or
                        ('\t' == fileData[splitPos - 1])))
                    {
                        splitPos--;
                    }

                    if (lineStart >= splitPos)
                    {
                        /* Bad ".INI" file */

                        return false;
                    }

                    fileData[splitPos] = '\0';

                    char const *const keyText =
                        (char const *) &(fileData[lineStart]);

                    char *const valueText =
                        (char *) &(fileData[valueStart]);

                    if (0 == strcmp(keyText, "ip"))
                    {
                        *serverIP = valueText;
                    }
                    else if (0 == strcmp(keyText, "port"))
                    {
                        *serverPort = valueText;
                    }
                    else if (0 == strcmp(keyText, "pass"))
                    {
                        *serverPass = valueText;
                    }
                    else
                    {
                        /* Bad ".INI" file */

                        return false;
                    }
                }
            }

            lineStart = i + 1;
        }
    }

    return true;
}

/**************************************************************/

void
kao2mp_Server_LoadConfig(
    SOCKADDR_IN *sockAddrIn)
{
    char *serverIP   = NULL;
    char *serverPort = NULL;
    char *serverPass = NULL;

    bool  test;
    BYTE  *fileData = NULL;
    DWORD fileSize  = 0;

    /* Open the config file */

    uintptr_t file = CreateFileA(
        MULTIPLAYER_CONFIG_SERVER,
        GENERIC_READ,
        0,
        0,
        OPEN_EXISTING,
        0,
        0);

    if (INVALID_HANDLE_VALUE == file)
    {
        MessageBoxA(
            0,
            "Server Config: File \""
                MULTIPLAYER_CONFIG_SERVER
                "\" not found!\n\n"
                "Loading default settings.",
            "SERVER.EXE",
            MB_ICONWARNING);
    }
    else while (1)
    {
        fileSize = GetFileSize(file, NULL);

        if ((INVALID_FILE_SIZE == fileSize) or (0 == fileSize))
        {
            MessageBoxA(
                0,
                "Server Config: Bad file size!\n\n"
                    "Loading default settings.",
                "SERVER.EXE",
                MB_ICONWARNING);

            fileSize = 0;
            break;
        }

        fileData = mallocWrapper(1 + fileSize);

        if (NULL == fileData)
        {
            MessageBoxA(
                0,
                "Server Config: Memory allocation error!\n\n"
                    "Loading default settings.",
                "SERVER.EXE",
                MB_ICONWARNING);

            fileSize = 0;
            break;
        }

        test = ReadFile(file, fileData, fileSize, NULL, 0);

        if (not test)
        {
            MessageBoxA(
                0,
                "Server Config: File read error!\n\n"
                    "Loading default settings.",
                "SERVER.EXE",
                MB_ICONWARNING);

            fileSize = 0;
            break;
        }

        test = kao2mp_Server_ParseConfig(
            fileData,
            fileSize,
            &(serverIP),
            &(serverPort),
            &(serverPass));

        if (not test)
        {
            MessageBoxA(
                0,
                "Server Config: Bad file format!\n\n"
                    "Loading default settings.",
                "SERVER.EXE",
                MB_ICONWARNING);

            serverIP   = NULL;
            serverPort = NULL;
            serverPass = NULL;

            fileSize = 0;
            break;
        }

        break;
    }

    if (INVALID_HANDLE_VALUE != file)
    {
        CloseHandle(file);
    }

    /* Load settings (serverIP) */

    if (NULL != serverIP)
    {
        ADDRESS_FAMILY af = sockAddrIn->sin_family;

        if (1 != inet_pton(af, serverIP, &(sockAddrIn->sin_addr)))
        {
            /* Bad "IPv4" text, use default */

            serverIP = NULL;
        }
    }

    if (NULL == serverIP)
    {
        sockAddrIn->sin_addr.s_addr = INADDR_ANY;
    }

    /* Load settings (serverPort) */

    if (NULL != serverPort)
    {
        int x = Dummy_TextToInt32(serverPort);

        if ((x < 0) || (x >= (1 << 16)))
        {
            /* Bad port value, use default */

            serverPort = NULL;
        }
        else
        {
            sockAddrIn->sin_port = htons((uint16_t) x);
        }
    }

    if (NULL == serverPort)
    {
        sockAddrIn->sin_port = htons(MULTIPLAYER_DEFAULT_PORT);
    }

    /* Load settings (serverPass) */

    if (NULL != serverPass)
    {
        size_t const passLen = strlen(serverPass);

        if ((passLen < MULTIPLAYER_PASS_LENGTH_MIN) ||
            (passLen > MULTIPLAYER_PASS_LENGTH_MAX))
        {
            MessageBoxA(
                0,
                "Server Config: Bad passPhrase length!" "\n"
                    "Loading default passPhrase.",
                "SERVER.EXE",
                MB_ICONWARNING);

            serverPass = NULL;
        }
        else
        {
            gServerPass = _strdup(serverPass);
        }
    }

    if (NULL == serverPass)
    {
        gServerPass = _strdup(MULTIPLAYER_DEFAULT_PASS);
    }

    /* Clear memory */

    if (NULL != fileData)
    {
        freeWrapper((uintptr_t) fileData);
    }
}

/**************************************************************/

void
kao2mp_Server_ShowError(
    char const *text,
    uintptr_t  context)
{
    char errorMsg[256];

    char numericBuf[2 + 8 + 1];

    /* Construct the message "text: 0x00000000" */

    errorMsg[0] = '\0';

    strcat(errorMsg, text);

    if (0 == context)
    {
        strcat(errorMsg, ".");
    }
    else
    {
        strcat(errorMsg, ": ");

        Dummy_UInt32ToText(numericBuf, context);

        strcat(errorMsg, numericBuf);
    }

    /* Display MessageBox */

    MessageBoxA(0, errorMsg, "SERVER.EXE", MB_ICONWARNING);
}

/**************************************************************/
