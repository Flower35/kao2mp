/**
 * "SERVER/SERVER.H"
 */

#include <iso646.h>
#include <stdint.h>
#include <stdbool.h>

#include "COMMON/WIN32.H"
#include "COMMON/WINSOCK2.H"

#include "COMMON/KAO2.H"
#include "COMMON/KAO2MP.H"

/**************************************************************/
/* kao2mp Server : external data                              */
/**************************************************************/

extern kao2_PlayerServerData *gPlayers;
extern int gPlayersCount;
extern int gPlayersAlloc;

extern char *gServerPass;

/**************************************************************/
/* "C" declarations                                           */
/**************************************************************/

extern unsigned _fltused;

int
__cdecl
memcmp(void const *, void const *, size_t);

void *
__cdecl
memcpy(void *, void const *, size_t);

char *
__cdecl
strcat(char *, char const *);

char *
__cdecl
_strdup(char const *);

float
__cdecl
_lltof(int64_t);

/**************************************************************/
/* kao2mp Server : procedures                                 */
/**************************************************************/

void *
mallocWrapper(
    size_t size);

void
freeWrapper(
    uintptr_t mem);

void
PrintMessageA(
    char const *message);

uint32_t
kao2mp_Server_CalcChksum(
    void const *data,
    size_t size);

uint32_t
kao2mp_Server_CalcNewUId(void);

int32_t
kao2mp_Server_GetNumActivePlayers(void);

int32_t
kao2mp_Server_GetFirstFreeIndex(void);

void
kao2mp_Server_ResetPlayer(
    int const index);

bool
kao2mp_Server_SetPlayersArraySize(
    int newPlayerCount);

void
Dummy_UInt16ToTextDec(
    char *buf,
    uint16_t value);

void
Dummy_UInt32ToText(
    char *buf,
    uintptr_t value);

int32_t
Dummy_TextToInt32(
    char const *buf);

void
kao2mp_Server_PrintAddressInfo(
    SOCKADDR_IN const *sockAddrIn);

void
kao2mp_Server_LoadConfig(
    SOCKADDR_IN *sockAddrIn);

void
kao2mp_Server_ShowError(
    char const *text,
    uintptr_t  context);

/**************************************************************/
