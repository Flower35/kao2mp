/**
 * "KAO2PLUS/CLIENT2.CPP"
 */

#include "KAO2PLUS.HPP"

extern "C"
{
    #include "COMMON/WINSOCK2.H"
    #include "COMMON/KAO2MP.H"
}

/**************************************************************/

extern int32_t gClientIndex;

extern SOCKADDR_IN gServerSockAddr;

extern kao2_PlayerClientData *gPlayers;

/**************************************************************/

void
kao2mp_Client_UpdateRemotePlayerNodes(
    int const i)
{
    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const xformType = moduleBase + (gContext->typeInfo_eTransform);
    uintptr_t const meshType  = moduleBase + (gContext->typeInfo_eTriMesh);

    uintptr_t const heroNode = gPlayers[i].heroNode;

    uintptr_t node;

    /**
     * | angelWings | (eTransform) "+angel_wing" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+angel_wing");

    if (0 != node)
    {
        gPlayers[i].data.nodes.angelWings ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | flying_helmet1 | (eTransform) "+helmet_u" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+helmet_u");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet1 ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | flying_helmet2 | (eTransform) "helmet_wing_dr" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "helmet_wing_dr");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet2 ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | flying_helmet3 | (eTransform) "helmet_wing_dl" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "helmet_wing_dl");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet3 ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | flying_helmet4 | (eTransform) "+helmet_wing_r" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+helmet_wing_r");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet4 ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | flying_helmet5 | (eTransform) "+helmet_wing_l" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+helmet_wing_l");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet5 ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | skakanka | (eTransform) "+skak_root" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+skak_root");

    if (0 != node)
    {
        gPlayers[i].data.nodes.skakanka ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | skakanka_l | (eTransform) "+skak_lewa" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+skak_lewa");

    if (0 != node)
    {
        gPlayers[i].data.nodes.skakanka_l ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | skakanka_r | (eTransform) "+skak_prawa" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+skak_prawa");

    if (0 != node)
    {
        gPlayers[i].data.nodes.skakanka_r ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | budzik | (eTransform) "+budzik" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+budzik");

    if (0 != node)
    {
        gPlayers[i].data.nodes.budzik ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | boomerang | (eTriMesh) "boomerang / main" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        meshType,
        "boomerang / main + boomerang / main + boomerang / main");

    if (0 != node)
    {
        gPlayers[i].data.nodes.boomerang ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | butla01 | (eTriMesh) "+butla" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        meshType,
        "+butla");

    if (0 != node)
    {
        gPlayers[i].data.nodes.butla01 ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }

    /**
     * | pistol | (eTransform) "+water_gun" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+water_gun");

    if (0 != node)
    {
        gPlayers[i].data.nodes.pistol ?
            (kao2_eNode_enable(node)) : (kao2_eNode_disable(node));
    }
}

/**************************************************************/

void
kao2mp_Client_UpdateLocalPlayerNodes(void)
{
    int const i = gClientIndex;

    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const xformType = moduleBase + (gContext->typeInfo_eTransform);
    uintptr_t const meshType  = moduleBase + (gContext->typeInfo_eTriMesh);

    uintptr_t const heroNode = kao2_Game_GetHero();

    uintptr_t node;

    /**
     * | angelWings | (eTransform) "+angel_wing" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+angel_wing");

    if (0 != node)
    {
        gPlayers[i].data.nodes.angelWings = kao2_eNode_isEnabled(node);
    }

    /**
     * | flying_helmet1 | (eTransform) "+helmet_u" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+helmet_u");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet1 = kao2_eNode_isEnabled(node);
    }

    /**
     * | flying_helmet2 | (eTransform) "helmet_wing_dr" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "helmet_wing_dr");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet2 = kao2_eNode_isEnabled(node);
    }

    /**
     * | flying_helmet3 | (eTransform) "helmet_wing_dl" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "helmet_wing_dl");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet3 = kao2_eNode_isEnabled(node);
    }

    /**
     * | flying_helmet4 | (eTransform) "+helmet_wing_r" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+helmet_wing_r");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet4 = kao2_eNode_isEnabled(node);
    }

    /**
     * | flying_helmet5 | (eTransform) "+helmet_wing_l" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+helmet_wing_l");

    if (0 != node)
    {
        gPlayers[i].data.nodes.flying_helmet5 = kao2_eNode_isEnabled(node);
    }

    /**
     * | skakanka | (eTransform) "+skak_root" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+skak_root");

    if (0 != node)
    {
        gPlayers[i].data.nodes.skakanka = kao2_eNode_isEnabled(node);
    }

    /**
     * | skakanka_l | (eTransform) "+skak_lewa" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+skak_lewa");

    if (0 != node)
    {
        gPlayers[i].data.nodes.skakanka_l = kao2_eNode_isEnabled(node);
    }

    /**
     * | skakanka_r | (eTransform) "+skak_prawa" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+skak_prawa");

    if (0 != node)
    {
        gPlayers[i].data.nodes.skakanka_r = kao2_eNode_isEnabled(node);
    }

    /**
     * | budzik | (eTransform) "+budzik" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+budzik");

    if (0 != node)
    {
        gPlayers[i].data.nodes.budzik = kao2_eNode_isEnabled(node);
    }

    /**
     * | boomerang | (eTriMesh) "boomerang / main" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        meshType,
        "boomerang / main + boomerang / main + boomerang / main");

    if (0 != node)
    {
        gPlayers[i].data.nodes.boomerang = kao2_eNode_isEnabled(node);
    }

    /**
     * | butla01 | (eTriMesh) "+butla" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        meshType,
        "+butla");

    if (0 != node)
    {
        gPlayers[i].data.nodes.butla01 = kao2_eNode_isEnabled(node);
    }

    /**
     * | pistol | (eTransform) "+water_gun" |
     */
    node = kao2_eGroup_findExact(
        heroNode,
        xformType,
        "+water_gun");

    if (0 != node)
    {
        gPlayers[i].data.nodes.pistol = kao2_eNode_isEnabled(node);
    }
}

/**************************************************************/

void
kao2mp_Client_AddStatusMsg_Connected(void)
{
    char buffer[KAO2PLUS__STATUS_MSG__LENGTH + 1];
    char addrBuf[INET_ADDRSTRLEN];
    char numericBuf[5 + 1];

    int addrBufSize = INET_ADDRSTRLEN;

    /* Socket Address as a text */

    addrBuf[0] = '\0';

    inet_ntop(
        AF_INET,
        &(gServerSockAddr.sin_addr),
        addrBuf,
        &(addrBufSize));

    /* Socket Port as a text */

    Dummy_UInt16ToTextDec(
        numericBuf,
        ntohs(gServerSockAddr.sin_port));

    /* Concatenate strings */

    buffer[0] = '\0';

    strcat(buffer, "Connected to (");
    strcat(buffer, addrBuf);
    strcat(buffer,  " : ");
    strcat(buffer, numericBuf);
    strcat(buffer, ")");

    /* Copy and show the message */

    kao2_Game_AddStatusMsg(buffer);
}

/**************************************************************/

void
kao2mp_Client_AddStatusMsg_PlayerJoined(
    int const index)
{
    char buffer[KAO2PLUS__STATUS_MSG__LENGTH + 1];

    /* Concatenate strings */

    buffer[0] = '\0';

    strcat(buffer, "Player (");
    strcat(buffer, gPlayers[index].data.name);
    strcat(buffer, ") joined");

    /* Copy and show the message */

    kao2_Game_AddStatusMsg(buffer);
}

/**************************************************************/

void
kao2mp_Client_AddStatusMsg_PlayerLeft(
    int const index)
{
    char buffer[KAO2PLUS__STATUS_MSG__LENGTH + 1];

    /* Concatenate strings */

    buffer[0] = '\0';

    strcat(buffer, "Player (");
    strcat(buffer, gPlayers[index].data.name);
    strcat(buffer, ") disconnected");

    /* Copy and show the message */

    kao2_Game_AddStatusMsg(buffer);
}

/**************************************************************/

void
kao2mp_Client_AddStatusMsg_PlayerChangedLevel(
    int const index)
{
    char buffer[KAO2PLUS__STATUS_MSG__LENGTH + 1];

    int32_t const levelId = gPlayers[index].data.levelId;

    /* Concatenate strings */

    buffer[0] = '\0';

    strcat(buffer, "Player (");
    strcat(buffer, gPlayers[index].data.name);
    strcat(buffer, ") entered (");
    strcat(buffer, kao2_Game_GetLevelName(levelId));
    strcat(buffer, ")");

    /* Copy and show the message */

    kao2_Game_AddStatusMsg(buffer);
}

/**************************************************************/
