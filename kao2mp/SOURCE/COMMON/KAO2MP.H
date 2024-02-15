/**
 * "COMMON/KAO2MP.H"
 */

/**************************************************************/
/* Default settings                                           */
/**************************************************************/

#define MULTIPLAYER_DEFAULT_IP    "127.0.0.1"
#define MULTIPLAYER_DEFAULT_PORT  27015
#define MULTIPLAYER_DEFAULT_PASS  "123"
#define MULTIPLAYER_DEFAULT_NICK  "Kao"

#define MULTIPLAYER_CONFIG_SERVER  "SERVER.INI"
#define MULTIPLAYER_CONFIG_CLIENT  "CLIENT.INI"

#define MULTIPLAYER_TIMEOUT_SECONDS  10
#define MULTIPLAYER_PASS_LENGTH_MIN   3
#define MULTIPLAYER_PASS_LENGTH_MAX  32

/**************************************************************/
/* Messages between the Client and the Server                 */
/**************************************************************/

#define MULTIPLAYER_ERROR_OK     0
#define MULTIPLAYER_ERROR_RETRY  1
#define MULTIPLAYER_ERROR_FATAL  (-1)

#define MULTIPLAYER_CLIENT_CONNECT       10
#define MULTIPLAYER_CLIENT_REQUEST_DATA  20
#define MULTIPLAYER_CLIENT_SEND_PLAYER   30

#define MULTIPLAYER_SERVER_ACCEPT        40
#define MULTIPLAYER_SERVER_SEND_COUNT    50
#define MULTIPLAYER_SERVER_SEND_PLAYER   60

/**************************************************************/
/* Player Data                                                */
/**************************************************************/

typedef struct kao2_DenisNodes kao2_DenisNodes;

struct kao2_DenisNodes
{
    bool angelWings;
    bool flying_helmet1;
    bool flying_helmet2;
    bool flying_helmet3;
    bool flying_helmet4;
    bool flying_helmet5;
    bool skakanka;
    bool skakanka_l;
    bool skakanka_r;
    bool budzik;
    bool boomerang;
    bool butla01;
    bool pistol;
};

#define PLAYER_NICKNAME_LENGTH  (24 - 1)

typedef struct kao2_PlayerData kao2_PlayerData;

struct kao2_PlayerData
{
    bool          active;
    char          name[1 + PLAYER_NICKNAME_LENGTH];
    kao2_ColorRGB color;

    uint32_t gameVersion;
    int32_t  levelId;

    kao2_eSRP       xform;
    int32_t         animId;
    float           animFrame;
    kao2_DenisNodes nodes;
};

/**************************************************************/
/* Player Data sent over the Internet                         */
/**************************************************************/

typedef struct kao2_PlayerNetworkData kao2_PlayerNetworkData;

struct kao2_PlayerNetworkData
{
    uint32_t chksum;
    kao2_PlayerData data;
};

/**************************************************************/
/* Player Data kept on the Sever side                         */
/**************************************************************/

typedef struct kao2_PlayerServerData kao2_PlayerServerData;

struct kao2_PlayerServerData
{
    uint32_t uid;
    float timeOut;

    kao2_PlayerNetworkData data;
};

/**************************************************************/
/* Player Data kept on the Client side                        */
/**************************************************************/

typedef struct kao2_PlayerClientData kao2_PlayerClientData;

struct kao2_PlayerClientData
{
    uintptr_t heroNode;
    uintptr_t textFieldNode;
    
    bool prevActive;
    int32_t prevLevelId;

    kao2_PlayerData data;
};

/**************************************************************/
/* Client Config Fields                                       */
/**************************************************************/

typedef struct kao2_ClientConfig kao2_ClientConfig;

struct kao2_ClientConfig
{
    struct
    {
        char *ip;
        char *port;
        char *pass;
    }
    server;
    
    struct
    {
        char *name;
        char *colorR;
        char *colorG;
        char *colorB;
    }
    client;
};

/**************************************************************/
