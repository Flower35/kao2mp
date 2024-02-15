/**
 * "KAO2PLUS/CLIENT3.CPP"
 */

#include "KAO2PLUS.HPP"

extern "C"
{
    #include "COMMON/WINSOCK2.H"
    #include "COMMON/KAO2MP.H"
}

/**************************************************************/

extern char *gServerPass;
extern char *gClientNickname;
extern kao2_ColorRGB gClientColor;

extern SOCKADDR_IN gServerSockAddr;

/**************************************************************/

bool
kao2mp_Client_ParseConfig(
    BYTE *fileData,
    int32_t fileSize,
    kao2_ClientConfig *config)
{
    /**
     * [server]
     *   ip = 127.0.0.1
     *   port = 65535
     *   pass = Kao
     *
     * [client]
     *   nick = Kao
     *   colorR = 255
     *   colorG = 128
     *   colorB = 0
     */

    bool isServerTag = false;
    bool isClientTag = false;

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
                    isClientTag = false;

                    char const *const tagName =
                        (char const *) &(fileData[lineStart + 1]);

                    if (0 == strcmp(tagName, "server"))
                    {
                        isServerTag = true;
                    }
                    else if (0 == strcmp(tagName, "client"))
                    {
                        isClientTag = true;
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

                    if (isServerTag)
                    {
                        if (0 == strcmp(keyText, "ip"))
                        {
                            config->server.ip = valueText;
                        }
                        else if (0 == strcmp(keyText, "port"))
                        {
                            config->server.port = valueText;
                        }
                        else if (0 == strcmp(keyText, "pass"))
                        {
                            config->server.pass = valueText;
                        }
                        else
                        {
                            /* Bad ".INI" file */

                            return false;
                        }
                    }
                    else if (isClientTag)
                    {
                        if (0 == strcmp(keyText, "nick"))
                        {
                            config->client.name = valueText;
                        }
                        else if (0 == strcmp(keyText, "colorR"))
                        {
                            config->client.colorR = valueText;
                        }
                        else if (0 == strcmp(keyText, "colorG"))
                        {
                            config->client.colorG = valueText;
                        }
                        else if (0 == strcmp(keyText, "colorB"))
                        {
                            config->client.colorB = valueText;
                        }
                        else
                        {
                            /* Bad ".INI" file */

                            return false;
                        }
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

bool
kao2mp_Client_LoadConfig(void)
{
    WCHAR filePathBuf[MAX_PATH];

    MallocFunc const mallocFunc = kao2_Game_GetMallocFunc();
    FreeFunc const freeFunc = kao2_Game_GetFreeFunc();

    SOCKADDR_IN *const sockAddrIn = &(gServerSockAddr);

    kao2_ClientConfig config;

    config.server.ip     = NULL;
    config.server.port   = NULL;
    config.server.pass   = NULL;
    config.client.name   = NULL;
    config.client.colorR = NULL;
    config.client.colorG = NULL;
    config.client.colorB = NULL;

    bool  test;
    BYTE  *fileData;
    DWORD fileSize;

    /* Open the config file */

    {
        filePathBuf[0] = L'\0';

        DWORD length = GetDllDirectoryW(sizeof(filePathBuf), filePathBuf);

        filePathBuf[length] = L'\\';

        wcscpy(
            &(filePathBuf[length + 1]),
            (WCHAR const *) TEXT(MULTIPLAYER_CONFIG_CLIENT));
    }

    uintptr_t file = CreateFileW(
        filePathBuf,
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
            "Client Config: File \""
                MULTIPLAYER_CONFIG_CLIENT
                "\"not found!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        return false;
    }

    fileSize = GetFileSize(file, NULL);

    if ((INVALID_FILE_SIZE == fileSize) or (0 == fileSize))
    {
        MessageBoxA(
            0,
            "Client Config: Bad file size!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        CloseHandle(file);
        return false;
    }

    fileData = (BYTE *) mallocFunc(1 + fileSize);

    if (NULL == fileData)
    {
        MessageBoxA(
            0,
            "Client Config: Memory allocation error!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        CloseHandle(file);
        return false;
    }

    test = ReadFile(file, fileData, fileSize, NULL, 0);

    if (not test)
    {
        MessageBoxA(
            0,
            "Client Config: File read error!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        freeFunc((uint32_t) fileData);
        CloseHandle(file);
        return false;
    }

    test = kao2mp_Client_ParseConfig(fileData, fileSize, &(config));

    CloseHandle(file);

    if (not test)
    {
        MessageBoxA(
            0,
            "Client Config: Bad file format!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        freeFunc((uint32_t) fileData);
        return false;
    }

    /* Load settings (config.server.ip) */

    ADDRESS_FAMILY af = AF_INET;

    sockAddrIn->sin_family = AF_INET;

    if (NULL != config.server.ip)
    {
        if (1 != inet_pton(af, config.server.ip, &(sockAddrIn->sin_addr)))
        {
            /* Bad "IPv4" text, use default */

            config.server.ip = NULL;
        }
    }

    if (NULL == config.server.ip)
    {
        inet_pton(af, MULTIPLAYER_DEFAULT_IP, &(sockAddrIn->sin_addr));
    }

    /* Load settings (config.server.port) */

    if (NULL != config.server.port)
    {
        int x = Dummy_TextToInt32(config.server.port);

        if ((x < 0) || (x >= (1 << 16)))
        {
            /* Bad port value, use default */

            config.server.port = NULL;
        }
        else
        {
            sockAddrIn->sin_port = htons((uint16_t) x);
        }
    }

    if (NULL == config.server.port)
    {
        sockAddrIn->sin_port = htons(MULTIPLAYER_DEFAULT_PORT);
    }

    /* Load settings (config.server.pass) */

    if (NULL == config.server.pass)
    {
        MessageBoxA(
            0,
            "Client Config: Missing passPhrase!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        freeFunc((uint32_t) fileData);
        return false;
    }

    size_t const passLen = strlen(config.server.pass);

    if ((passLen < MULTIPLAYER_PASS_LENGTH_MIN) ||
        (passLen > MULTIPLAYER_PASS_LENGTH_MAX))
    {
        MessageBoxA(
            0,
            "Client Config: Bad passPhrase length!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        freeFunc((uint32_t) fileData);
        return false;
    }

    gServerPass = _strdup(config.server.pass);

    /* Load settings (config.client.name) */

    if (NULL != config.client.name)
    {
        gClientNickname = _strdup(config.client.name);
    }
    else
    {
        gClientNickname = _strdup(MULTIPLAYER_DEFAULT_NICK);
    }

    /* Load settings (config.client.colorR */

    if (NULL != config.client.colorR)
    {
        int x = Dummy_TextToInt32(config.client.colorR);

        if (x < 0) { x = 0; } else if (x > 255) { x = 255; }

        gClientColor.r = x / 255.f;
    }
    else
    {
        gClientColor.r = 1.0f;
    }

    /* Load settings (config.client.colorG */

    if (NULL != config.client.colorG)
    {
        int x = Dummy_TextToInt32(config.client.colorG);

        if (x < 0) { x = 0; } else if (x > 255) { x = 255; }

        gClientColor.g = x / 255.f;
    }
    else
    {
        gClientColor.g = 1.0f;
    }

    /* Load settings (config.client.colorR */

    if (NULL != config.client.colorB)
    {
        int x = Dummy_TextToInt32(config.client.colorB);

        if (x < 0) { x = 0; } else if (x > 255) { x = 255; }

        gClientColor.b = x / 255.f;
    }
    else
    {
        gClientColor.b = 1.0f;
    }

    /* Clear memory */

    freeFunc((uintptr_t) fileData);

    return true;
}

/**************************************************************/
