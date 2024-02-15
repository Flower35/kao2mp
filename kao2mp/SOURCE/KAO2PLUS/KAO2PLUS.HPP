/**
 * "KAO2PLUS/KAO2PLUS.HPP"
 */

/**************************************************************/
/* "C" declarations (types, libraries, helper procedures)     */
/**************************************************************/

#include <cstdint>

extern "C"
{
    extern unsigned _fltused;
}

extern "C"
{
    #include "COMMON/WIN32.H"
    #include "COMMON/KAO2.H"
}

extern "C"
{
    void *
    __cdecl
    memcpy(void *, void const *, size_t);

    int
    __cdecl
    memcmp(void const *, void const *, size_t);

    size_t
    __cdecl
    strlen(char const *);

    int
    __cdecl
    strcmp(char const *, char const *);

    char const *
    __cdecl
    strstr(char const*, char const*);

    char *
    __cdecl
    strcat(char *, char const *);

    WCHAR *
    __cdecl
    wcscpy(WCHAR *, WCHAR const *);

    char *
    __cdecl
    strncpy(char *, char const *, size_t);

    long
    __cdecl
    _ftol(double);

    float
    __cdecl
    _fmodf(float, float);

    char *
    __cdecl
    _strdup(char const *);
}

/**************************************************************/
/* Kao2 Function Types                                        */
/**************************************************************/

typedef uintptr_t (__cdecl *NewObjectFunc)(void);

typedef uintptr_t (__cdecl *GetTypeInfoFunc)(void);

typedef uintptr_t (__thiscall *CloneFunc)(
    uintptr_t cloner,
    uintptr_t object);

typedef uintptr_t (__cdecl *MallocFunc)(
    uintptr_t size);

typedef void (__cdecl *FreeFunc)(
    uintptr_t address);

typedef uintptr_t (__thiscall *AppendNodeFunc)(
    uintptr_t self,
    uintptr_t node,
    int index);

typedef uintptr_t (__thiscall *DetachNodeFunc)(
    uintptr_t self,
    uintptr_t node);

typedef void (__thiscall *DeletingDtor)(
    uintptr_t node,
    bool      unknown);

typedef void (__thiscall *TextFieldCtorFunc)(
    uintptr_t self,
    int32_t maxCharCount);

typedef void (__thiscall *TextFieldSetColorFunc)(
    uintptr_t self,
    float colorR,
    float colorG,
    float colorB,
    float colorA,
    int count);

typedef void (__thiscall *TextFieldSetTextFunc)(
    uintptr_t self,
    char const *text,
    bool italics);

/**************************************************************/
/* Kao2 Game Context (stuff specific to different versions)   */
/**************************************************************/

typedef struct kao2_GameContext kao2_GameContext;

struct kao2_GameContext
{
    /* Procedures (RVA) */

    uintptr_t global_glutDisplayCallback;
    uintptr_t global_mallocWrapper;
    uintptr_t global_freeWrapper;
    uintptr_t global_clone;

    uintptr_t textField_ctor;
    uintptr_t textField_setColor;
    uintptr_t textField_setText;

    /* Global Data (RVA) */

    uintptr_t gamelet;
    uintptr_t quitFlag;
    uintptr_t dinputKeyb;
    uintptr_t levelList;

    uintptr_t typeInfo_eNode;
    uintptr_t typeInfo_eGroup;
    uintptr_t typeInfo_eTransform;
    uintptr_t typeInfo_eBillboard;
    uintptr_t typeInfo_ePivot;
    uintptr_t typeInfo_eTriMesh;

    uintptr_t vft_eGroup;
    uintptr_t vft_ePivot;

    /* Internal differences */

    uintptr_t gamelet_orthoNode;
};

/**************************************************************/
/* "Kao2Plus" Status Messages                                 */
/**************************************************************/

#define KAO2PLUS__STATUS_MSG__TOTAL_NODES    5
#define KAO2PLUS__STATUS_MSG__TOTAL_BUFFER  10

#define KAO2PLUS__STATUS_MSG__LENGTH  (96 - 1)

#define KAO2PLUS__STATUS_MSG__TIMEOUT   3
#define KAO2PLUS__STATUS_MSG__FADE_TIME  1

typedef struct kao2_StatusMsg kao2_StatusMsg;

struct kao2_StatusMsg
{
    float timeOut;
    char text[KAO2PLUS__STATUS_MSG__LENGTH + 1];
};

/**************************************************************/
/* "Kao2Plus" external data                                   */
/**************************************************************/

extern kao2_GameContext const *gContext;
extern uint32_t gGameVersion;

extern kao2_GameContext const gKao2_Retail_Context;
extern kao2_GameContext const gKao2_Steamless_Context;
extern kao2_GameContext const gKao2_Digital_Context;

/**************************************************************/
/* Generic Helper Procedures                                  */
/**************************************************************/

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

uintptr_t
GetModuleHandle(void);

/**************************************************************/
/* Game Data Procedures, Game Object Procedures               */
/**************************************************************/

uintptr_t
kao2_Game_GetQuitFlag(void);

void
kao2_Game_SetQuitFlag(
    uintptr_t value);

int32_t
kao2_Game_GetLevelId(void);

char const *
kao2_Game_GetLevelName(
    int32_t index);

uintptr_t
kao2_Game_GetGamelet(void);

uintptr_t
kao2_Game_GetScene(void);

uintptr_t
kao2_Game_GetHero(void);

MallocFunc
kao2_Game_GetMallocFunc(void);

FreeFunc
kao2_Game_GetFreeFunc(void);

void
kao2_eString_decRef(
    uintptr_t strObj);

uintptr_t
kao2_eString_create(
    char const *text);

char const *
kao2_eString_getText(
    uintptr_t strObj);

bool
kao2_TypeInfo_isDerivedFrom(
    uintptr_t objectType,
    uintptr_t expectedType);

uintptr_t
kao2_TypeInfo_newObject(
    uintptr_t typeInfo);

uintptr_t
kao2_eObject_getTypeInfo(
    uintptr_t object);

bool
kao2_eNode_isEnabled(
    uintptr_t node);

void
kao2_eNode_disable(
    uintptr_t node);

void
kao2_eNode_enable(
    uintptr_t node);

char const *
kao2_eNode_getNameCStr(
    uintptr_t node);

void
kao2_eNode_setName(
    uintptr_t node,
    char const *newName);

bool
kao2_eNode_matchesExact(
    uintptr_t node,
    uintptr_t typeInfo,
    char const *name);

void
kao2_eNode_appendToScene(
    uintptr_t node,
    int index);

void
kao2_eNode_detachFromParent(
    uintptr_t node);

int32_t
kao2_eRefCounter_getRef(
    uintptr_t node);

int32_t
kao2_eRefCounter_incRef(
    uintptr_t node);

int32_t
kao2_eRefCounter_decRef(
    uintptr_t node);

uintptr_t
kao2_eGroup_findExact(
    uintptr_t groupNode,
    uintptr_t childTypeInfo,
    char const *childName);

void
kao2_eTransform_getSRP(
    uintptr_t node,
    kao2_eSRP *srpRef);

void
kao2_eTransform_setSRP(
    uintptr_t node,
    kao2_eSRP *srpRef);

bool
kao2_ePivot_getAnim(
    uintptr_t node,
    int *animIdRef,
    float *animTimerRef);

void
kao2_ePivot_setAnim(
    uintptr_t node,
    int animId,
    float animTimer);

/**************************************************************/
/* Other (advanced) Game Procedures                           */
/**************************************************************/

char const *
kao2_Game_WStrToCStr(
    WCHAR const *utf16Text);

int
kao2_Game_PokeMemory(
    uintptr_t lpAddress,
    void const *lpcSource,
    DWORD dwSize);

bool
kao2_eNode_Cloning_Restore(
    uintptr_t node,
    uintptr_t *buffer);

bool
kao2_eNode_Cloning_StoreAndHide(
    uintptr_t node,
    uintptr_t *buffer);

bool
kao2_eNode_Cloning_Count(
    uintptr_t node);

void
kao2_eGroup_Cloning_Restore(
    uintptr_t node,
    uintptr_t **buffer);

void
kao2_eGroup_Cloning_StoreAndHide(
    uintptr_t node,
    uintptr_t **buffer);

int
kao2_eGroup_Cloning_Count(
    uintptr_t node);

uintptr_t
kao2_Game_CloneHero(void);

bool
kao2_Game_CreateNicknameTextField(
    uintptr_t heroNode,
    uintptr_t *textFieldRef,
    size_t capacity);

void
kao2_Game_UpdateNicknameTextField(
    uintptr_t textField,
    kao2_ColorRGB const *color,
    char const *text);

void
kao2_Game_AddStatusMsg(
    char const *text);

bool
kao2_Game_CreateStatusMsgTextField(
    uintptr_t parentNode,
    uintptr_t *textFieldRef,
    int32_t index);

void
kao2_Game_UpdateStatusMsgTextField(
    uintptr_t textField,
    char const *text,
    float alpha);

/**************************************************************/
/* Networking                                                 */
/**************************************************************/

bool
kao2mp_Client_LoadConfig(void);

bool
kao2mp_Client_GameLoopHandler(void);

DWORD
__stdcall
kao2mp_Client_ThreadProc(
    uintptr_t reserved);

void
kao2mp_Client_CleanUp(void);

/**************************************************************/
