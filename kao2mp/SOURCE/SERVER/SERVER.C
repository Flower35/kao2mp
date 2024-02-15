/**
 * "SERVER/SERVER.C"
 */

#include "SERVER.H"

/**************************************************************/

kao2_PlayerServerData *gPlayers;
int gPlayersCount;
int gPlayersAlloc;

char *gServerPass;

/**************************************************************/

bool
kao2mp_Server_Run(void)
{
    char numericBuf[5 + 1];

    WSADATA wsaData;

    SOCKET gServerSocket;
    SOCKADDR_IN gServerSockAddr;
    SOCKADDR gClientSockAddr;

    /* Welcome Message */

    PrintMessageA("********************************\n");
    PrintMessageA("* kao2mp Server                *\n");
    PrintMessageA("********************************\n");

    PrintMessageA("\n");

    /* Initialize WinSock */

    int result = WSAStartup(WINSOCK_VERSION, &(wsaData));

    if (0 != result)
    {
        kao2mp_Server_ShowError("WSAStartup() failed", WSAGetLastError());

        return false;
    }

    /* Create Socket */

    gServerSocket = socket(
        AF_INET,
        SOCK_DGRAM,
        IPPROTO_UDP);

    if (INVALID_SOCKET == gServerSocket)
    {
        kao2mp_Server_ShowError("socket() failed", WSAGetLastError());

        return false;
    }

    /* Load Server Config */

    gServerSockAddr.sin_family = AF_INET;

    kao2mp_Server_LoadConfig(&(gServerSockAddr));

    /* Print Server Info */

    PrintMessageA("* Listening at ");
    kao2mp_Server_PrintAddressInfo(&(gServerSockAddr));
    PrintMessageA("\n");

    PrintMessageA("* PassPhrase is \"");
    PrintMessageA(gServerPass);
    PrintMessageA("\"\n\n");

    /* Bind Socket */

    result = bind(
        gServerSocket,
        (SOCKADDR const *) &(gServerSockAddr),
        sizeof(gServerSockAddr));

    if (SOCKET_ERROR == result)
    {
        kao2mp_Server_ShowError("bind() failed", WSAGetLastError());

        return false;
    }

    /**
     * Common Buffer
     * (TODO: please assert (bufferSize >= MULTIPLAYER_PASS_LENGTH_MAX))
     */
    #define bufferHeadSize  (3 * sizeof(int32_t))
    #define bufferDataSize  (sizeof(kao2_PlayerNetworkData))
    #define bufferSize      (bufferHeadSize + bufferDataSize)

    BYTE buffer[bufferSize];

    int32_t *bufferHead = (int32_t *) buffer;

    kao2_PlayerNetworkData *networkPlayer =
        (kao2_PlayerNetworkData *) &(buffer[bufferHeadSize]);

    /* Last Client Info */

    int sockAddrLen;

    /* Server Loop */

    LARGE_INTEGER perfFreq;
    perfFreq.QuadPart = 1;
    QueryPerformanceFrequency(&(perfFreq));

    LARGE_INTEGER perfCount[2];
    perfCount[0].QuadPart = 0;
    QueryPerformanceCounter(&(perfCount[0]));

    while (1)
    {
        /* Receive some data from Any Client */

        bufferHead[0] = (-1);
        bufferHead[1] = (-1);
        bufferHead[2] = (-1);

        sockAddrLen = sizeof(SOCKADDR);

        result = recvfrom(
            gServerSocket,
            (char *) buffer,
            sizeof(buffer),
            0,
            &(gClientSockAddr),
            &(sockAddrLen));

        if (SOCKET_ERROR == result)
        {
            kao2mp_Server_ShowError("recvfrom() failed", WSAGetLastError());

            return false;
        }

        /* Measure elapsed time */

        perfCount[1].QuadPart = 0;
        QueryPerformanceCounter(&(perfCount[1]));

        float const deltaTime =
            _lltof(perfCount[1].QuadPart - perfCount[0].QuadPart) /
            _lltof(perfFreq.QuadPart);

        perfCount[0] = perfCount[1];

        /* Check for Client Timeouts */

        bool anyTimeOut = false;

        for (int i = 0; i < gPlayersCount; i++)
        {
            if (gPlayers[i].timeOut < 0) { continue; }

            float const timeOut = gPlayers[i].timeOut + deltaTime;

            if (timeOut >= MULTIPLAYER_TIMEOUT_SECONDS)
            {
                Dummy_UInt16ToTextDec(numericBuf, (uint16_t) (1 + i));

                PrintMessageA("* Player #");
                PrintMessageA(numericBuf);
                PrintMessageA(" timed out!\n");

                kao2mp_Server_ResetPlayer(i);
                anyTimeOut = true;
            }
            else
            {
                gPlayers[i].timeOut = timeOut;
            }
        }

        if (anyTimeOut)
        {
            PrintMessageA("* ( ");

            Dummy_UInt16ToTextDec(
                numericBuf,
                (uint16_t) kao2mp_Server_GetNumActivePlayers());

            PrintMessageA(numericBuf);
            PrintMessageA(" players left )\n");
        }

        /* Process Client Requests */

        switch (bufferHead[0])
        {
            /* New Client requests to join the Server */

            case MULTIPLAYER_CLIENT_CONNECT:
            {
                const size_t clientPassSize = bufferHead[2];

                if ((bufferHead[1] != 0) or
                    (clientPassSize < MULTIPLAYER_PASS_LENGTH_MIN) or
                    (clientPassSize > MULTIPLAYER_PASS_LENGTH_MAX))
                {
                    PrintMessageA(
                        "* CLIENT_CONNECT: Invalid request header\n");

                    break;
                }

                /* Check the passPhrase */

                const size_t serverPassSize = strlen(gServerPass);

                bool validPass = false;

                uint32_t playerUId   = 0;
                int32_t  playerIndex = (-1);

                if (clientPassSize == serverPassSize)
                {
                    result = memcmp(
                        &(buffer[bufferHeadSize]),
                        gServerPass,
                        serverPassSize);

                    validPass = (0 == result);
                }

                /* Respond to the connection request */

                if (validPass)
                {
                    bufferHead[0] = MULTIPLAYER_SERVER_ACCEPT;

                    playerIndex = kao2mp_Server_GetFirstFreeIndex();

                    /* Make space for further Player Data */

                    if (playerIndex >= gPlayersCount)
                    {
                        int const newPlayerCount = 1 + gPlayersCount;

                        if (not kao2mp_Server_SetPlayersArraySize(newPlayerCount))
                        {
                            kao2mp_Server_ShowError(
                                "SetPlayersArraySize() failed",
                                0);

                            return false;
                        }
                    }

                    playerUId = kao2mp_Server_CalcNewUId();

                    gPlayers[playerIndex].uid = playerUId;
                    gPlayers[playerIndex].timeOut = 0;
                }
                else
                {
                    bufferHead[0] = 0;
                }

                bufferHead[1] = playerUId;
                bufferHead[2] = playerIndex;

                result = sendto(
                    gServerSocket,
                    (char const *) bufferHead,
                    bufferHeadSize,
                    0,
                    &(gClientSockAddr),
                    sizeof(gClientSockAddr));

                if (SOCKET_ERROR == result)
                {
                    kao2mp_Server_ShowError(
                        "sendto() failed",
                        WSAGetLastError());

                    break;
                }

                /* Print Client Info */

                PrintMessageA("* CLIENT_CONNECT ( ");
                PrintMessageA(validPass ? "TRUE" : "FALSE");

                if (validPass)
                {
                    PrintMessageA(" ; ");

                    kao2mp_Server_PrintAddressInfo(
                        (SOCKADDR_IN const *) &(gClientSockAddr));

                    PrintMessageA(" ; #");

                    Dummy_UInt16ToTextDec(
                        numericBuf,
                        (uint16_t) (1 + playerIndex));

                    PrintMessageA(numericBuf);
                    PrintMessageA(" ; ");

                    Dummy_UInt16ToTextDec(
                        numericBuf,
                        (uint16_t) kao2mp_Server_GetNumActivePlayers());

                    PrintMessageA(numericBuf);
                    PrintMessageA(" players )\n");
                }
                else
                {
                    PrintMessageA(" )\n");
                }

                break;
            }

            /* Client transmits Player Data (with no response) */

            case MULTIPLAYER_CLIENT_SEND_PLAYER:
            {
                uint32_t const uid   = bufferHead[1];
                int32_t  const index = bufferHead[2];

                if ((index < 0) or
                    (index >= gPlayersCount) or
                    (uid != gPlayers[index].uid))
                {
                    PrintMessageA(
                        "* CLIENT_SEND_PLAYER: Invalid request header\n");

                    break;
                }

                /* Validate CheckSum */

                uint32_t chksum = kao2mp_Server_CalcChksum(
                    &(networkPlayer->data),
                    sizeof(networkPlayer->data));

                if (networkPlayer->chksum != chksum)
                {
                    PrintMessageA(
                        "* CLIENT_SEND_PLAYER: Bad Player Data chksum");

                    break;
                }

                /* Update Player Data at given index */

                gPlayers[index].data = *networkPlayer;

                /* Reset TimeOut counter */

                gPlayers[index].timeOut = 0;

                #if 0
                PrintMessageA("Processed CLIENT_SEND_PLAYER.\n");
                #endif

                break;
            }

            /* Client requests data of all other players */

            case MULTIPLAYER_CLIENT_REQUEST_DATA:
            {
                uint32_t const uid   = bufferHead[1];
                int32_t  const index = bufferHead[2];

                if ((index < 0) or
                    (index >= gPlayersCount) or
                    (uid != gPlayers[index].uid))
                {
                    PrintMessageA(
                        "* CLIENT_REQUEST_DATA: Invalid request header\n");

                    break;
                }

                /* First respond with the number of players */

                bufferHead[0] = MULTIPLAYER_SERVER_SEND_COUNT;
                bufferHead[1] = index;
                bufferHead[2] = gPlayersCount;

                result = sendto(
                    gServerSocket,
                    (char const *) bufferHead,
                    bufferHeadSize,
                    0,
                    &(gClientSockAddr),
                    sizeof(gClientSockAddr));

                if (SOCKET_ERROR == result)
                {
                    kao2mp_Server_ShowError(
                        "sendto() failed",
                        WSAGetLastError());

                    break;
                }

                /* Send each Player Data (skipping the client index) */

                bufferHead[0] = MULTIPLAYER_SERVER_SEND_PLAYER;

                for (int i = 0; i < gPlayersCount; i++)
                {
                    if (i == index) { continue; }

                    bufferHead[2] = i;

                    *networkPlayer = gPlayers[i].data;

                    result = sendto(
                        gServerSocket,
                        (char const *) buffer,
                        sizeof(buffer),
                        0,
                        &(gClientSockAddr),
                        sizeof(gClientSockAddr));

                    if (SOCKET_ERROR == result)
                    {
                        kao2mp_Server_ShowError(
                            "sendto() failed",
                            WSAGetLastError());

                        break;
                    }
                }

                /* Reset TimeOut counter */

                gPlayers[index].timeOut = 0;

                #if 0
                PrintMessageA("Processed CLIENT_REQUEST_DATA.\n");
                #endif

                break;
            }

            /* Ignore other unrecognized requests */
        }
    }

    return true;
}

/**************************************************************/

int
__stdcall
EntryPoint(void)
{
    bool result = kao2mp_Server_Run();

    ExitProcess(result ? 0 : 1);
}

/**************************************************************/
