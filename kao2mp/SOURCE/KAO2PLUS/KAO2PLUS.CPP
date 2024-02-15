/**
 * "KAO2PLUS/KAO2PLUS.CPP"
 */

#include "KAO2PLUS.HPP"

/**************************************************************/

#ifdef _DEBUG
bool gDebugKeyPressed[2];
int gDebugActionsCount;
#endif

uintptr_t gWorkerThreadHandle;

STATIC_ASSERT(KAO2PLUS__STATUS_MSG__TOTAL_NODES <=
    KAO2PLUS__STATUS_MSG__TOTAL_BUFFER);

int32_t gStatusMsgUsed;
uintptr_t gStatusMsgNodes[KAO2PLUS__STATUS_MSG__TOTAL_NODES];
kao2_StatusMsg gStatusMsgBuffers[KAO2PLUS__STATUS_MSG__TOTAL_BUFFER];
kao2_StatusMsg *gStatusMsgs[KAO2PLUS__STATUS_MSG__TOTAL_BUFFER];

/**************************************************************/

#ifdef _DEBUG
uintptr_t
Kao2Plus_GeneralTest(void)
{
    /* Try clonin a new hero */

    uintptr_t newHero = kao2_Game_CloneHero();

    if (0 == newHero) { return 0; }

    /* Try appending him to the current scene */

    kao2_eNode_appendToScene(newHero, 0);

    /* Try updating animation */

    uintptr_t oldHero = kao2_Game_GetHero();

    if (0 != oldHero)
    {
        int animId = 0;
        float animFrame = 0;

        if (kao2_ePivot_getAnim(oldHero, &(animId), &(animFrame)))
        {
            kao2_ePivot_setAnim(newHero, animId, animFrame);
        }
    }

    return newHero;
}
#endif

/**************************************************************/

void
kao2_Game_RemoveInvalidStatusMsgs(void)
{
    for (int i = 0; i < KAO2PLUS__STATUS_MSG__TOTAL_NODES; i++)
    {
        uintptr_t const textField = gStatusMsgNodes[i];

        if (0 == textField) { continue; }

        if (1 == kao2_eRefCounter_getRef(textField))
        {
            kao2_eRefCounter_decRef(textField);

            gStatusMsgNodes[i] = 0;
        }
    }
}

/**************************************************************/

void
kao2_Game_SetupStatusMsgs(void)
{
    for (int i = 0; i < KAO2PLUS__STATUS_MSG__TOTAL_NODES; i++)
    {
        gStatusMsgNodes[i] = 0;
    }

    for (int i = 0; i < KAO2PLUS__STATUS_MSG__TOTAL_BUFFER; i++)
    {
        gStatusMsgs[i] = &(gStatusMsgBuffers[i]);
        gStatusMsgs[i]->timeOut = 0;
    }

    gStatusMsgUsed = 0;
}

/**************************************************************/

void
kao2_Game_ShiftStatusMsgs(
    int startIndex)
{
    if (gStatusMsgUsed <= 0) { return; }

    int const lastIndex = (gStatusMsgUsed - 1);

    kao2_StatusMsg* firstStatusMsg = gStatusMsgs[startIndex];

    for (int i = startIndex; i < lastIndex; i++)
    {
        gStatusMsgs[i] = gStatusMsgs[i + 1];
    }

    gStatusMsgs[lastIndex] = firstStatusMsg;

    gStatusMsgUsed--;
}

/**************************************************************/

void
kao2_Game_AddStatusMsg(
    char const *text)
{
    bool inQueue = false;

    if (gStatusMsgUsed >= KAO2PLUS__STATUS_MSG__TOTAL_BUFFER)
    {
        kao2_Game_ShiftStatusMsgs(0);

        gStatusMsgUsed = (KAO2PLUS__STATUS_MSG__TOTAL_BUFFER - 1);
    }

    if (gStatusMsgUsed >= KAO2PLUS__STATUS_MSG__TOTAL_NODES)
    {
        inQueue = true;
    }

    kao2_StatusMsg *const statusMsg = gStatusMsgs[gStatusMsgUsed];

    uintptr_t const gamelet = kao2_Game_GetGamelet();

    if (inQueue)
    {
        statusMsg->timeOut = (-1);
    }
    else if (0 == gamelet)
    {
        statusMsg->timeOut = KAO2PLUS__STATUS_MSG__TIMEOUT;
    }
    else
    {
        float const sceneTime = *((float *)
            (gamelet + offsetof(kao2_eKao2Gamelet, sceneTime)));

        statusMsg->timeOut = sceneTime + KAO2PLUS__STATUS_MSG__TIMEOUT;
    }

    strncpy(statusMsg->text, text, KAO2PLUS__STATUS_MSG__LENGTH);

    statusMsg->text[KAO2PLUS__STATUS_MSG__LENGTH] = '\0';

    gStatusMsgUsed++;
}

/**************************************************************/

void
kao2_Game_UpdateStatusMsgsVis(void)
{
    uintptr_t const gamelet = kao2_Game_GetGamelet();

    if (0 == gamelet) { return; }

    uintptr_t const orthoNode = *((uintptr_t *)
        (gamelet + (gContext->gamelet_orthoNode)));

    if (0 == orthoNode) { return; }

    float const sceneTime = *((float *)
        (gamelet + offsetof(kao2_eKao2Gamelet, sceneTime)));

    /* Hide all available `eTextField` nodes */

    for (int i = 0; i < KAO2PLUS__STATUS_MSG__TOTAL_NODES; i++)
    {
        uintptr_t textField = gStatusMsgNodes[i];

        if (0 != textField)
        {
            uint32_t *const visAlpha = (uint32_t *)
                (textField + offsetof(kao2_eNode, visRate));

            *visAlpha = 0;
        }
    }

    /**
     * Iterate though each `eTextField` node,
     * update it according to each "Status Message" structure
     * ("text nodes count" LEQ "messages in queue")
     */
    int index = 0;

    for (int i = 0; i < KAO2PLUS__STATUS_MSG__TOTAL_NODES; i++)
    {
        if (index >= gStatusMsgUsed) { break; }

        kao2_StatusMsg *const statusMsg = gStatusMsgs[index];

        if (0 == (statusMsg->timeOut))
        {
            /* Choose next "Status Message", same `eTextField` node */

            kao2_Game_ShiftStatusMsgs(index);
            i--;
            continue;
        }

        uintptr_t const prevTextField = gStatusMsgNodes[i];

        bool test = kao2_Game_CreateStatusMsgTextField(
            orthoNode,
            &(gStatusMsgNodes[i]),
            i);

        if (not test)
        {
            kao2_Game_SetQuitFlag(1);
            return;
        }

        /**
         * Reset the TimeOut counter on two conditions:
         * - new `eTextField` has been just created
         * - status message was hidden in a queue
         */
        if ((prevTextField != gStatusMsgNodes[i]) or
            ((statusMsg->timeOut) < 0))
        {
            statusMsg->timeOut = sceneTime + KAO2PLUS__STATUS_MSG__TIMEOUT;
        }

        float alpha = 1.0f;

        if (sceneTime >= (statusMsg->timeOut))
        {
            float const fadeTime = sceneTime - (statusMsg->timeOut);

            if (fadeTime >= KAO2PLUS__STATUS_MSG__FADE_TIME)
            {
                alpha = 0;
                statusMsg->timeOut = 0;

                /* Is it the last loaded message? */

                if ((gStatusMsgUsed - 1) == index) { gStatusMsgUsed--; }
            }
            else
            {
                alpha = (KAO2PLUS__STATUS_MSG__FADE_TIME - fadeTime) /
                    KAO2PLUS__STATUS_MSG__FADE_TIME;
            }
        }

        kao2_Game_UpdateStatusMsgTextField(
            gStatusMsgNodes[i],
            statusMsg->text,
            alpha);

        /* Next "Status Message" and next `eTextField` */

        index++;
    }
}

/**************************************************************/

extern "C"
uintptr_t
__stdcall
Kao2Plus_GameLoopStart(void)
{
    #ifdef _DEBUG
    {
        uintptr_t const moduleBase = GetModuleHandle();

        /* Debugging! */

        uintptr_t const keyboardState = moduleBase + (gContext->dinputKeyb);

        BYTE *const flagPtr = (BYTE *) (keyboardState + DIK_LBRACKET);

        gDebugKeyPressed[0] = gDebugKeyPressed[1];
        gDebugKeyPressed[1] = (0 != (*flagPtr));

        if ((not gDebugKeyPressed[0]) and gDebugKeyPressed[1])
        {
            gDebugActionsCount++;

            Kao2Plus_GeneralTest();

            {
                char buffer[32];
                char numericBuf[5 + 1];

                Dummy_UInt16ToTextDec(
                    numericBuf,
                    (uint16_t) gDebugActionsCount);

                buffer[0] = '\0';
                strcat(buffer, "Hero Clone ");
                strcat(buffer, numericBuf);
                strcat(buffer,  " added");

                kao2_Game_AddStatusMsg(buffer);
            }
        }
    }
    #endif

    /* Update "Status Messages" visibility */

    kao2_Game_UpdateStatusMsgsVis();

    /* Return TRUE if the game should close */

    if (not kao2mp_Client_GameLoopHandler()) { return TRUE; }

    return (1 == kao2_Game_GetQuitFlag());
}

/**************************************************************/

void
Kao2Plus_UnpatchOnExit(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    /* Restore code patched at "GLUT Display Callback" */

    int const payloadSize = 7;
    int const insOffset   = 6;

    BYTE payload[payloadSize];

    uintptr_t const researchAddr =
        moduleBase + (gContext->global_glutDisplayCallback) + insOffset;

    uintptr_t const quitFlagAddr =
        moduleBase + (gContext->quitFlag);

    /**
     * | 83 3D ?? ?? ?? ?? 00 | cmp dword ptr [<quitFlagAddr>],0 |
     */
    payload[0] = 0x83;
    payload[1] = 0x3D;
    payload[2] = ((quitFlagAddr)       & 0xFF);
    payload[3] = ((quitFlagAddr >>  8) & 0xFF);
    payload[4] = ((quitFlagAddr >> 16) & 0xFF);
    payload[5] = ((quitFlagAddr >> 24) & 0xFF);
    payload[6] = 0x00;

    (void) kao2_Game_PokeMemory(researchAddr, payload, payloadSize);
}

/**************************************************************/

int
Kao2Plus_PatchOnEntry(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    /* Detect Game Version */

    uintptr_t const peHeader = moduleBase + *((uintptr_t *)
        (moduleBase + 0x3C));

    uintptr_t const timeStamp = *((uintptr_t *)
        (peHeader + 0x08));

    switch (timeStamp)
    {
        case KAO2_RETAIL_TIMESTAMP:
        {
            gContext = &(gKao2_Retail_Context);
            break;
        }

        case KAO2_STEAMLESS_TIMESTAMP:
        {
            gContext = &(gKao2_Steamless_Context);
            break;
        }

        case KAO2_DIGITAL_TIMESTAMP:
        {
            gContext = &(gKao2_Digital_Context);
            break;
        }

        default:
        {
            MessageBoxA(
                0,
                "\"Kao2Plus_PatchOnEntry()\":" "\n\n"
                    "Unknown game version.",
                "Kao2Plus.DLL",
                MB_ICONWARNING);

        return 1;
        }
    }

    gGameVersion = timeStamp;

    /* Compare bytes at "GLUT Display Callback" */

    int const payloadSize = 7;
    int const insOffset   = 6;
    int const callInsLen  = 5;

    BYTE payload[payloadSize];

    uintptr_t const researchAddr =
        moduleBase + (gContext->global_glutDisplayCallback) + insOffset;

    uintptr_t const quitFlagAddr =
        moduleBase + (gContext->quitFlag);

    uintptr_t const jmpDestAddr =
        ((uintptr_t) Kao2Plus_GameLoopStart) - (researchAddr + callInsLen);

    /**
     * | 83 3D ?? ?? ?? ?? 00 | cmp dword ptr [<quitFlagAddr>],0 |
     */
    payload[0] = 0x83;
    payload[1] = 0x3D;
    payload[2] = ((quitFlagAddr)       & 0xFF);
    payload[3] = ((quitFlagAddr >>  8) & 0xFF);
    payload[4] = ((quitFlagAddr >> 16) & 0xFF);
    payload[5] = ((quitFlagAddr >> 24) & 0xFF);
    payload[6] = 0x00;

    if (0 != memcmp(payload, (void const *) researchAddr, payloadSize))
    {
        MessageBoxA(
            0,
            "\"Kao2Plus_PatchOnEntry()\":" "\n\n"
                "Already patched or broken game.",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        return 1;
    }

    /**
     * | E8 ?? ?? ?? ?? | call <GameLoopStart> |
     * | 85 C0          | test eax,eax         |
     */
    payload[0] = 0xE8;
    payload[1] = ((jmpDestAddr)       & 0xFF);
    payload[2] = ((jmpDestAddr >>  8) & 0xFF);
    payload[3] = ((jmpDestAddr >> 16) & 0xFF);
    payload[4] = ((jmpDestAddr >> 24) & 0xFF);
    payload[5] = 0x85;
    payload[6] = 0xC0;

    if (0 != kao2_Game_PokeMemory(researchAddr, payload, payloadSize))
    {
        MessageBoxA(
            0,
            "\"Kao2Plus_PatchOnEntry()\":" "\n\n"
                "\"PokeGameMemory\" failed!",
            "Kao2Plus.DLL",
            MB_ICONWARNING);

        return 1;
    }

    /* Skip the "Logo Land" intro */

    if (KAO2_RETAIL_TIMESTAMP == timeStamp)
    {
        uintptr_t const logoLoopAddr = moduleBase + 0x9F1A;
        /**
         * | 0F 8E 47 FF FF FF | jle ? |
         */
        payload[0] = 0x0F;
        payload[1] = 0x8E;

        if (0 == memcmp(payload, (void const *) logoLoopAddr, 2))
        {
            /**
             * | EB 0D | jmp ? |
             */
            payload[0] = 0xEB;
            payload[1] = 0x0D;

            (void) kao2_Game_PokeMemory(logoLoopAddr, payload, 2);
        }
    }
    else
    {
        uintptr_t const logoLoopAddr = moduleBase + 0x000165DD;
        /**
         * | 0F 8F F0 00 00 00 | jg ? |
         */
        payload[0] = 0x0F;
        payload[1] = 0x8F;

        if (0 == memcmp(payload, (void const *) logoLoopAddr, 2))
        {
            /**
             * | 90             | nop   |
             * | E9 F0 00 00 00 | jmp ? |
             */
            payload[0] = 0x90;
            payload[1] = 0xE9;

            (void) kao2_Game_PokeMemory(logoLoopAddr, payload, 2);
        }
    }

    return 0;
}

/**************************************************************/

extern "C"
int
__stdcall
DllMain(
    uintptr_t handle,
    unsigned reason,
    uintptr_t reserved)
{
    (void) handle;
    (void) reserved;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
        {
            kao2_Game_SetupStatusMsgs();

            /* Detect game version, patch "GLUT Display Calback" */

            if (0 != Kao2Plus_PatchOnEntry())
            {
                return FALSE;
            }

            /* Load Client configuration file */

            if (not kao2mp_Client_LoadConfig())
            {
                Kao2Plus_UnpatchOnExit();

                return FALSE;
            }

            /* Create Client Worker Thread */

            gWorkerThreadHandle = CreateThread(
                0,
                0,
                kao2mp_Client_ThreadProc,
                0,
                0,
                NULL);

            if (0 == gWorkerThreadHandle)
            {
                MessageBoxA(
                    0,
                    "Failed to create Client-Server thread!",
                    "Kao2Plus.DLL",
                    MB_ICONWARNING);

                Kao2Plus_UnpatchOnExit();

                return FALSE;
            }

            /* Display a welcome message. */

            #ifdef _DEBUG
            MessageBoxA(
                0,
                "Hello World!",
                "Kao2Plus.DLL",
                MB_ICONINFORMATION);
            #endif

            kao2_Game_AddStatusMsg("Hello World!");

            break;
        }

        case DLL_PROCESS_DETACH:
        {
            Kao2Plus_UnpatchOnExit();

            if (0 != gWorkerThreadHandle)
            {
                TerminateThread(gWorkerThreadHandle, 0);
            }

            kao2mp_Client_CleanUp();

            kao2_Game_RemoveInvalidStatusMsgs();

            break;
        }
    }

    return TRUE;
}

/**************************************************************/
