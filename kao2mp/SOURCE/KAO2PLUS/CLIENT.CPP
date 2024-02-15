/**
 * "KAO2PLUS/CLIENT.CPP"
 */

#include "KAO2PLUS.HPP"

extern "C"
{
    #include "COMMON/WINSOCK2.H"
    #include "COMMON/KAO2MP.H"
}

void
kao2mp_Client_UpdateRemotePlayerNodes(
    int const index);

void
kao2mp_Client_UpdateLocalPlayerNodes(void);

void
kao2mp_Client_AddStatusMsg_Connected(void);

void
kao2mp_Client_AddStatusMsg_PlayerJoined(
    int const index);

void
kao2mp_Client_AddStatusMsg_PlayerLeft(
    int const index);

void
kao2mp_Client_AddStatusMsg_PlayerChangedLevel(
    int const index);

/**************************************************************/

bool gClientConnected;

int32_t gClientIndex;
uint32_t gClientUId;

char *gServerPass;
char *gClientNickname;
kao2_ColorRGB gClientColor;

SOCKET gClientSocket;
SOCKADDR_IN gServerSockAddr;

kao2_PlayerClientData *gPlayers;
int gPlayersCount;
int gPlayersAlloc;

/**************************************************************/

void
kao2mp_Client_ShowError(
    char const *text,
    uintptr_t  context)
{
    char errorMsg[256];

    char numericBuf[2 + 8 + 1];

    /* Construct the message "text: 0x00000000" */

    errorMsg[0] = '\0';

    strcat(errorMsg, "[ Client Thread Error ]\n\n");

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

    MessageBoxA(0, errorMsg, "Kao2Plus.DLL", MB_ICONWARNING);
}

/**************************************************************/

uint32_t
kao2mp_Client_CalcChksum(
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

void
kao2mp_Client_SetPlayersArraySize(
    int newPlayerCount)
{
    MallocFunc const mallocFunc = kao2_Game_GetMallocFunc();
    FreeFunc const freeFunc = kao2_Game_GetFreeFunc();

    size_t const allocSize = newPlayerCount * sizeof(kao2_PlayerClientData);

    if (NULL == gPlayers)
    {
        gPlayers = (kao2_PlayerClientData *) mallocFunc(allocSize);

        gPlayersAlloc = newPlayerCount;
    }
    else if (newPlayerCount > gPlayersAlloc)
    {
        kao2_PlayerClientData *newPlayers =
            (kao2_PlayerClientData *) mallocFunc(allocSize);

        for (int i = 0; i < gPlayersCount; i++)
        {
            newPlayers[i] = gPlayers[i];
        }

        freeFunc((uintptr_t) gPlayers);

        gPlayers = newPlayers;

        gPlayersAlloc = newPlayerCount;
    }

    for (int i = gPlayersCount; i < newPlayerCount; i++)
    {
        gPlayers[i].heroNode = 0;
        gPlayers[i].textFieldNode = 0;

        gPlayers[i].prevActive = false;
        gPlayers[i].prevLevelId = (-1);

        gPlayers[i].data.active = false;
        gPlayers[i].data.levelId = (-1);
    }

    gPlayersCount = newPlayerCount;
}

/**************************************************************/

bool
kao2mp_Client_GetRemotePlayers(void)
{
    size_t const bufferHeadSize = 3 * sizeof(int32_t);
    size_t const bufferDataSize = sizeof(kao2_PlayerNetworkData);

    BYTE buffer[bufferHeadSize + bufferDataSize];

    int32_t *bufferHead = (int32_t *) buffer;

    kao2_PlayerNetworkData *networkPlayer =
        (kao2_PlayerNetworkData *) &(buffer[bufferHeadSize]);

    /* Send request (number of players) */

    bufferHead[0] = MULTIPLAYER_CLIENT_REQUEST_DATA;
    bufferHead[1] = gClientUId;
    bufferHead[2] = gClientIndex;

    int result = sendto(
        gClientSocket,
        (char const *) bufferHead,
        bufferHeadSize,
        0,
        (SOCKADDR const *) &(gServerSockAddr),
        sizeof(gServerSockAddr));

    if (SOCKET_ERROR == result)
    {
        kao2mp_Client_ShowError(
            "GetRemotePlayers, sendto() failed",
            WSAGetLastError());

        return false;
    }

    /* Get response (number of players) */

    bufferHead[0] = (-1);
    bufferHead[1] = (-1);
    bufferHead[2] = (-1);

    result = recvfrom(
        gClientSocket,
        (char *) bufferHead,
        bufferHeadSize,
        0,
        NULL,
        NULL);

    int const newPlayerCount = bufferHead[2];

    if (SOCKET_ERROR == result)
    {
        kao2mp_Client_ShowError(
            "GetRemotePlayers, recvfrom() failed",
            WSAGetLastError());

        return false;
    }

    if ((bufferHead[0] != MULTIPLAYER_SERVER_SEND_COUNT) or
        (bufferHead[1] != gClientIndex) or
        (newPlayerCount < 1))
    {
        kao2mp_Client_ShowError(
            "GetRemotePlayers, bad server response",
            0);

        return false;
    }

    /* Extend local array if needed */

    kao2mp_Client_SetPlayersArraySize(newPlayerCount);

    /* Receive each player */

    for (int i = 0; i < gPlayersCount; i++)
    {
        if (i == gClientIndex) { continue; }

        bufferHead[0] = (-1);
        bufferHead[1] = (-1);
        bufferHead[2] = (-1);

        result = recvfrom(
            gClientSocket,
            (char *) buffer,
            sizeof(buffer),
            0,
            NULL,
            NULL);

        if (SOCKET_ERROR == result)
        {
            kao2mp_Client_ShowError(
                "GetRemotePlayers, recvfrom() failed",
                WSAGetLastError());

            return false;
        }

        if ((bufferHead[0] != MULTIPLAYER_SERVER_SEND_PLAYER) or
            (bufferHead[1] != gClientIndex) or
            (bufferHead[2] != i))
        {
            kao2mp_Client_ShowError(
                "GetRemotePlayers, bad server response",
                0);

            return false;
        }

        uint32_t chksum = kao2mp_Client_CalcChksum(
            &(networkPlayer->data),
            sizeof(networkPlayer->data));

        if (networkPlayer->chksum != chksum)
        {
            kao2mp_Client_ShowError(
                "GetRemotePlayers, Bad Player Data chksum",
                0);

            return false;
        }

        gPlayers[i].data = networkPlayer->data;

        gPlayers[i].data.name[PLAYER_NICKNAME_LENGTH] = '\0';
    }

    return true;
}

/**************************************************************/

bool
kao2mp_Client_SendLocalPlayer(void)
{
    size_t const bufferHeadSize = 3 * sizeof(int32_t);
    size_t const bufferDataSize = sizeof(kao2_PlayerNetworkData);

    BYTE buffer[bufferHeadSize + bufferDataSize];

    int32_t *bufferHead = (int32_t *) buffer;

    kao2_PlayerNetworkData *networkPlayer =
        (kao2_PlayerNetworkData *) &(buffer[bufferHeadSize]);

    /* Prepare Request Hader */

    int const index = gClientIndex;

    bufferHead[0] = MULTIPLAYER_CLIENT_SEND_PLAYER;
    bufferHead[1] = gClientUId;
    bufferHead[2] = index;

    /* Prepare Server Player */

    networkPlayer->data = gPlayers[index].data;

    networkPlayer->chksum = kao2mp_Client_CalcChksum(
        &(networkPlayer->data),
        sizeof(networkPlayer->data));

    /* Send data */

    int result = sendto(
        gClientSocket,
        (char const *) buffer,
        sizeof(buffer),
        0,
        (SOCKADDR const *) &(gServerSockAddr),
        sizeof(gServerSockAddr));

    if (SOCKET_ERROR == result)
    {
        kao2mp_Client_ShowError(
            "SendLocalPlayer, sendto() failed",
            WSAGetLastError());

        return false;
    }

    return true;
}

/**************************************************************/

bool
kao2mp_Client_UpdateRemotePlayer(
    int const index)
{
    /* Messages */

    if ((not gPlayers[index].prevActive) and gPlayers[index].data.active)
    {
        kao2mp_Client_AddStatusMsg_PlayerJoined(index);
    }
    else if (gPlayers[index].prevActive and (not gPlayers[index].data.active))
    {
        kao2mp_Client_AddStatusMsg_PlayerLeft(index);
    }

    if (gPlayers[index].data.active and
        (gPlayers[index].prevLevelId != gPlayers[index].data.levelId))
    {
        kao2mp_Client_AddStatusMsg_PlayerChangedLevel(index);
    }

    gPlayers[index].prevActive = gPlayers[index].data.active;
    gPlayers[index].prevLevelId = gPlayers[index].data.levelId;

    /* Hero visualisation in-game */

    uintptr_t heroNode = gPlayers[index].heroNode;

    if (not gPlayers[index].data.active)
    {
        if (0 != heroNode) { kao2_eNode_disable(heroNode); }

        return true;
    }

    if (kao2_Game_GetLevelId() != gPlayers[index].data.levelId)
    {
        if (0 != heroNode) { kao2_eNode_disable(heroNode); }

        return true;
    }

    if (0 == kao2_Game_GetHero()) { return true; }

    if (0 == heroNode)
    {
        heroNode = kao2_Game_CloneHero();

        if (0 == heroNode)
        {
            /* CloneHero() failed. */

            return false;
        }

        kao2_eNode_appendToScene(heroNode, 0);

        if (2 != kao2_eRefCounter_incRef(heroNode))
        {
            /* AppendToScene() failed. */

            return false;
        }

        gPlayers[index].heroNode = heroNode;
    }

    kao2_eNode_enable(heroNode);

    /* Update nickname field (create `eTextField` if it does not exist) */

    bool test = kao2_Game_CreateNicknameTextField(
        heroNode,
        &(gPlayers[index].textFieldNode),
        PLAYER_NICKNAME_LENGTH);

    if (not test)
    {
        /* CreateNicknameTextField() failed. */

        return false;
    }

    kao2_Game_UpdateNicknameTextField(
        gPlayers[index].textFieldNode,
        &(gPlayers[index].data.color),
        gPlayers[index].data.name);

    /* Set other visual data */

    kao2_eTransform_setSRP(
        heroNode,
        &(gPlayers[index].data.xform));

    kao2_ePivot_setAnim(
        heroNode,
        gPlayers[index].data.animId,
        gPlayers[index].data.animFrame);

    kao2mp_Client_UpdateRemotePlayerNodes(index);

    return true;
}

/**************************************************************/

void
kao2mp_Client_UpdateLocalPlayer(void)
{
    int const index = gClientIndex;

    uintptr_t const heroNode = kao2_Game_GetHero();

    if (0 == heroNode)
    {
        gPlayers[index].data.active = false;
    }
    else
    {
        gPlayers[index].data.active = true;

        /* Set the player nickname field */

        strncpy(
            gPlayers[index].data.name,
            gClientNickname,
            PLAYER_NICKNAME_LENGTH);

        gPlayers[index].data.name[PLAYER_NICKNAME_LENGTH] = '\0';

        /* Set the player color field */

        gPlayers[index].data.color = gClientColor;

        /* Set game version and level ID */

        gPlayers[index].data.gameVersion = gGameVersion;

        gPlayers[index].data.levelId = kao2_Game_GetLevelId();

        /* Set other visual data */

        kao2_eTransform_getSRP(
            heroNode,
            &(gPlayers[index].data.xform));

        kao2_ePivot_getAnim(
            heroNode,
            &(gPlayers[index].data.animId),
            &(gPlayers[index].data.animFrame));

        kao2mp_Client_UpdateLocalPlayerNodes();
    }
}

/**************************************************************/

bool
kao2mp_Client_Init(void)
{
    /* Initialize WinSock */

    WSADATA wsaData;

    int result = WSAStartup(WINSOCK_VERSION, &(wsaData));

    if (0 != result)
    {
        kao2mp_Client_ShowError(
            "Init, WSAStartup() failed",
            WSAGetLastError());

        return false;
    }

    /* Create Socket */

    gClientSocket = socket(
        AF_INET,
        SOCK_DGRAM,
        IPPROTO_UDP);

    if (INVALID_SOCKET == gClientSocket)
    {
        kao2mp_Client_ShowError(
            "Init, socket() failed",
            WSAGetLastError());

        return false;
    }

    return true;
}

/**************************************************************/

int
kao2mp_Client_Connect(void)
{
    /* Send connection request */

    size_t const bufferHeadSize = 3 * sizeof(int32_t);

    size_t const passLen = strlen(gServerPass);

    BYTE buffer[bufferHeadSize + MULTIPLAYER_PASS_LENGTH_MAX];

    int32_t *const bufferHead = (int32_t *) buffer;

    bufferHead[0] = MULTIPLAYER_CLIENT_CONNECT;
    bufferHead[1] = 0;
    bufferHead[2] = passLen;

    memcpy(&(bufferHead[3]), gServerPass, passLen);

    int result = sendto(
        gClientSocket,
        (char const *) buffer,
        (bufferHeadSize + passLen),
        0,
        (SOCKADDR const *) &(gServerSockAddr),
        sizeof(gServerSockAddr));

    if (SOCKET_ERROR == result)
    {
        kao2mp_Client_ShowError(
            "Connect, sendto() failed",
            WSAGetLastError());

        return MULTIPLAYER_ERROR_FATAL;
    }

    /* Check if server accepted */

    bufferHead[0] = (-1);
    bufferHead[1] = (-1);
    bufferHead[2] = (-1);

    result = recvfrom(
        gClientSocket,
        (char *) bufferHead,
        bufferHeadSize,
        0,
        NULL,
        NULL);

    if (SOCKET_ERROR == result)
    {
        kao2mp_Client_ShowError(
            "Connect, recvfrom() failed",
            WSAGetLastError());

        return MULTIPLAYER_ERROR_FATAL;
    }

    if ((bufferHead[0] != MULTIPLAYER_SERVER_ACCEPT) or
        (0 == bufferHead[1]) or
        (bufferHead[2] < 0))
    {
        kao2mp_Client_ShowError(
            "Connect, request rejected by server",
            0);

        return MULTIPLAYER_ERROR_RETRY;
    }

    gClientUId   = bufferHead[1];
    gClientIndex = bufferHead[2];

    return MULTIPLAYER_ERROR_OK;
}

/**************************************************************/

void
kao2mp_Client_RemoveInvalidHeroes(
    bool checkTextFields)
{
    for (int i = 0; i < gPlayersCount; i++)
    {
        if (i == gClientIndex) { continue; }

        uintptr_t const heroNode = gPlayers[i].heroNode;

        if (0 == heroNode) { continue; }

        if (1 == kao2_eRefCounter_getRef(heroNode))
        {
            kao2_eRefCounter_decRef(heroNode);

            gPlayers[i].heroNode = 0;
        }
    }

    if (not checkTextFields) { return; }

    for (int i = 0; i < gPlayersCount; i++)
    {
        if (i == gClientIndex) { continue; }

        uintptr_t const textField = gPlayers[i].textFieldNode;

        if (0 == textField) { continue; }

        if (1 == kao2_eRefCounter_getRef(textField))
        {
            kao2_eRefCounter_decRef(textField);

            gPlayers[i].textFieldNode = 0;
        }
    }
}

/**************************************************************/

bool
kao2mp_Client_GameLoopHandler(void)
{
    if (gClientConnected)
    {
        /**
         * First check if Heroes have bad reference counters
         * (local player respawned or level changed)
         */
        kao2mp_Client_RemoveInvalidHeroes(false);

        /* Update remote Heroes (data -> cloned nodes) */

        for (int i = 0; i < gPlayersCount; i++)
        {
            if (i == gClientIndex) { continue; }

            if (not kao2mp_Client_UpdateRemotePlayer(i)) { return false; }
        }

        /* Update local Hero (hero node -> data) */

        kao2mp_Client_UpdateLocalPlayer();
    }

    return true;
}

/**************************************************************/

DWORD
__stdcall
kao2mp_Client_ThreadProc(
    uintptr_t reserved)
{
    (void) reserved;

    if (not kao2mp_Client_Init())
    {
        kao2_Game_SetQuitFlag(1);
        return 0;
    }

    while (1)
    {
        if (not gClientConnected)
        {
            gClientIndex = (-1);
            gPlayersCount = 0;

            while (1)
            {
                int const result = kao2mp_Client_Connect();

                if (MULTIPLAYER_ERROR_FATAL == result)
                {
                    kao2_Game_SetQuitFlag(1);
                    return 0;
                }
                else if (MULTIPLAYER_ERROR_OK == result)
                {
                    kao2mp_Client_AddStatusMsg_Connected();
                    break;
                }
            }

            /* Enough space to hold Local Player data */

            if (gClientIndex >= gPlayersCount)
            {
                int const newPlayersCount = 1 + gPlayersCount;

                kao2mp_Client_SetPlayersArraySize(newPlayersCount);
            }
            else
            {
                /* Possibly reconnecting! */

                uintptr_t const heroNode = gPlayers[gClientIndex].heroNode;

                if (0 != heroNode)
                {
                    /**
                     * Unconditionally remove cloned node.
                     * Local player checks only the real "$hero" Actor.
                     */
                    kao2_eRefCounter_decRef(heroNode);

                    gPlayers[gClientIndex].heroNode = 0;
                }
            }

            for (int i = 0; i < gPlayersCount; i++)
            {
                gPlayers[i].data.active = false;
            }

            gClientConnected = true;
        }

        while (gClientConnected)
        {
            /* Load remote player data from the server */

            if (not kao2mp_Client_GetRemotePlayers())
            {
                gClientConnected = false;
                break;
            }

            /* Send local player data to the server */

            if (not kao2mp_Client_SendLocalPlayer())
            {
                gClientConnected = false;
                break;
            }

            break;
        }
    }

    return 0;
}

/**************************************************************/

void
kao2mp_Client_CleanUp(void)
{
    gClientConnected = false;

    closesocket(gClientSocket);

    {
        FreeFunc const freeFunc = kao2_Game_GetFreeFunc();

        if (NULL != gServerPass)
        {
            freeFunc((uintptr_t) gServerPass);
        }

        if (NULL != gClientNickname)
        {
            freeFunc((uintptr_t) gClientNickname);
        }

        kao2mp_Client_RemoveInvalidHeroes(true);

        if (NULL != gPlayers)
        {
            freeFunc((uintptr_t) gPlayers);
        }
    }

    WSACleanup();
}

/**************************************************************/
