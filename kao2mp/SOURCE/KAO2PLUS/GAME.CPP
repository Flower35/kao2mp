/**
 * "KAO2PLUS/GAME.CPP"
 */

#include "KAO2PLUS.HPP"

/**************************************************************/

uintptr_t
kao2_Game_GetQuitFlag(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    return *((uintptr_t *)
        (moduleBase + (gContext->quitFlag)));
}

/**************************************************************/

void
kao2_Game_SetQuitFlag(
    uintptr_t value)
{
    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t *const ref = (uintptr_t *)
        (moduleBase + (gContext->quitFlag));

    *ref = value;
}

/**************************************************************/

int32_t
kao2_Game_GetLevelId(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    return *((uintptr_t *)
        (moduleBase + (gContext->levelList) + 0x0C));
}

/**************************************************************/

char const *
kao2_Game_GetLevelName(
    int32_t index)
{
    uintptr_t const moduleBase = GetModuleHandle();

    int32_t numLevels = *((int32_t *)
        (moduleBase + (gContext->levelList)));

    if ((index < 0) or (index >= numLevels)) { return ""; }

    uintptr_t *const levelsArray = *((uintptr_t **)
        (moduleBase + (gContext->levelList) + 0x08));

    uintptr_t const strObj = *((uintptr_t *)
        (levelsArray[index] + 0x08));

    if (KAO2_RETAIL_TIMESTAMP == gGameVersion)
    {
        return kao2_eString_getText(strObj);
    }
    else
    {
        return kao2_Game_WStrToCStr(
            (WCHAR const *) kao2_eString_getText(strObj));
    }
}

/**************************************************************/

uintptr_t
kao2_Game_GetGamelet(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    return *((uintptr_t *)
        (moduleBase + (gContext->gamelet)));
}

/**************************************************************/

uintptr_t
kao2_Game_GetScene(void)
{
    uintptr_t const gamelet = kao2_Game_GetGamelet();

    if (0 == gamelet) { return 0; }

    return *((uintptr_t *)
        (gamelet + offsetof(kao2_eKao2Gamelet, scene)));
}

/**************************************************************/

uintptr_t
kao2_Game_GetHero(void)
{
    uintptr_t const gamelet = kao2_Game_GetGamelet();

    if (0 == gamelet) { return 0; }

    return *((uintptr_t *)
        (gamelet + offsetof(kao2_eKao2Gamelet, hero)));
}

/**************************************************************/

MallocFunc
kao2_Game_GetMallocFunc(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    return (MallocFunc)
        (moduleBase + (gContext->global_mallocWrapper));
}

/**************************************************************/

FreeFunc
kao2_Game_GetFreeFunc(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    return (FreeFunc)
        (moduleBase + (gContext->global_freeWrapper));
}

/**************************************************************/

void
kao2_eString_decRef(
    uintptr_t strObj)
{
    int32_t *const counterRef = (int32_t *) strObj;

    (*counterRef)--;

    if (0 != (*counterRef)) { return; }

    /* We assume that the "global string" never runs our of references */

    (kao2_Game_GetFreeFunc())(strObj);
}

/**************************************************************/

uintptr_t
kao2_eString_create(
    char const *text)
{
    /* Calculate Total Object Size */

    int32_t const textLength = strlen(text);
    int32_t const arraySize  = 1 + textLength;

    uintptr_t const textOffset = sizeof(kao2_eStringHeader);

    size_t const objSize = textOffset + arraySize;

    /* Create new String Object */

    uintptr_t const strObj = (kao2_Game_GetMallocFunc())(objSize);

    kao2_eStringHeader *const strObjHead = (kao2_eStringHeader *) strObj;

    strObjHead->refCount   = 1;
    strObjHead->textLength = textLength;
    strObjHead->arraySize  = arraySize;

    memcpy((void *) (strObj + textOffset), text, arraySize);

    return strObj;
}

/**************************************************************/

char const *
kao2_eString_getText(
    uintptr_t strObj)
{
    uintptr_t const textOffset = sizeof(kao2_eStringHeader);

    return (char const *) (strObj + textOffset);
}

/**************************************************************/

bool
kao2_TypeInfo_isDerivedFrom(
    uintptr_t objectType,
    uintptr_t expectedType)
{
    while (0 != objectType)
    {
        if (objectType == expectedType) { return true; }

        objectType = *((uintptr_t *) (objectType + 0x08));
    }

    return false;
}

/**************************************************************/

uintptr_t
kao2_TypeInfo_newObject(
    uintptr_t typeInfo)
{
    NewObjectFunc const func = (NewObjectFunc)
        *((uintptr_t *) (typeInfo + 0x10));

    return func();
}

/**************************************************************/

uintptr_t
kao2_eObject_getTypeInfo(
    uintptr_t object)
{
    uintptr_t const vft = *((uintptr_t *) object);

    GetTypeInfoFunc func = (GetTypeInfoFunc) *((uintptr_t *) vft);

    return func();
}

/**************************************************************/

bool
kao2_eNode_isEnabled(
    uintptr_t node)
{
    uint32_t *const flagsRef = (uint32_t *)
        (node + offsetof(kao2_eNode, visFlags));

    return (0 != ((*flagsRef) & 0x08));
}

/**************************************************************/

void
kao2_eNode_disable(
    uintptr_t node)
{
    uint32_t *const flagsRef = (uint32_t *)
        (node + offsetof(kao2_eNode, visFlags));

    (*flagsRef) &= (compl ((uint32_t) 0x08));
}

/**************************************************************/

void
kao2_eNode_enable(
    uintptr_t node)
{
    uint32_t *const flagsRef = (uint32_t *)
        (node + offsetof(kao2_eNode, visFlags));

    (*flagsRef) |= 0x08;
}

/**************************************************************/

char const *
kao2_eNode_getNameCStr(
    uintptr_t node)
{
    uintptr_t const strObj = *((uintptr_t *)
        (node + offsetof(kao2_eNode, name)));

    return kao2_eString_getText(strObj);
}

/**************************************************************/

void
kao2_eNode_setName(
    uintptr_t node,
    char const *newName)
{
    uintptr_t *const strObjRef = (uintptr_t *)
        (node + offsetof(kao2_eNode, name));

    kao2_eString_decRef(*strObjRef);

    *strObjRef = kao2_eString_create(newName);
}

/**************************************************************/

bool
kao2_eNode_matchesExact(
    uintptr_t node,
    uintptr_t typeInfo,
    char const *name)
{
    uintptr_t const nodeType = kao2_eObject_getTypeInfo(node);

    if (nodeType != typeInfo) { return false; }

    char const *const nodeName = kao2_eNode_getNameCStr(node);

    return (0 == strcmp(nodeName, name));
}

/**************************************************************/

void
kao2_eNode_appendToScene(
    uintptr_t node,
    int index)
{
    /* Read `eScene` address */

    uintptr_t const scene = kao2_Game_GetScene();

    if (0 == scene) { return; }

    /* Read `eScene` vftable */

    uintptr_t const vft = *((uintptr_t *) scene);

    AppendNodeFunc const func = (AppendNodeFunc)
        *((uintptr_t *) (vft + 0x70));

    /* Append this node to the current scene */

    func(scene, node, index);
}

/**************************************************************/

void
kao2_eNode_detachFromParent(
    uintptr_t node)
{
    uintptr_t const parent = *((uintptr_t *)
        (node + offsetof(kao2_eNode, parent)));

    if (0 == parent) { return; }

    /* Read `eGroup` vftable */

    uintptr_t const vft = *((uintptr_t *) parent);

    DetachNodeFunc const func = (DetachNodeFunc)
        *((uintptr_t *) (vft + 0x74));

    /* Detach this node from its parent */

    func(parent, node);
}

/**************************************************************/

int32_t
kao2_eRefCounter_getRef(
    uintptr_t node)
{
    if (0 == node) { return (-1); }

    return *((int32_t *)
        (node + offsetof(kao2_eRefCounter, refCount)));
}

/**************************************************************/

int32_t
kao2_eRefCounter_incRef(
    uintptr_t node)
{
    if (0 == node) { return (-1); }

    int32_t *const counterRef = (int32_t *)
        (node + offsetof(kao2_eRefCounter, refCount));

    (*counterRef)++;

    return (*counterRef);
}

/**************************************************************/

int32_t
kao2_eRefCounter_decRef(
    uintptr_t node)
{
    if (0 == node) { return (-1); }

    int32_t *const counterRef = (int32_t *)
        (node + offsetof(kao2_eRefCounter, refCount));

    (*counterRef)--;

    if (0 == (*counterRef))
    {
        /* Call virtual destructor */

        uintptr_t const vft = *((uintptr_t *) node);

        DeletingDtor const dtor = (DeletingDtor)
            *((uintptr_t *) (vft + 0x0C));

        dtor(node, true);

        return 0;
    }

    return (*counterRef);
}

/**************************************************************/

uintptr_t
kao2_eGroup_findExact(
    uintptr_t groupNode,
    uintptr_t childTypeInfo,
    char const *childName)
{
    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const groupType = moduleBase + (gContext->typeInfo_eGroup);

    /* Iterate through children nodes */

    int const count = *((int *)
        (groupNode + offsetof(kao2_eGroup, count)));

    uintptr_t *const array = *((uintptr_t **)
        (groupNode + offsetof(kao2_eGroup, nodes)));

    for (int i = 0; i < count; i++)
    {
        bool test = kao2_eNode_matchesExact(
            array[i],
            childTypeInfo,
            childName);

        if (test) { return array[i]; }

        /* Child node might be a nested group */

        uintptr_t const typeInfo = kao2_eObject_getTypeInfo(array[i]);

        if (kao2_TypeInfo_isDerivedFrom(typeInfo, groupType))
        {
            uintptr_t result = kao2_eGroup_findExact(
                array[i],
                childTypeInfo,
                childName);

            if (0 != result) { return result; }
        }
    }

    return 0;
}

/**************************************************************/

void
kao2_eTransform_getSRP(
    uintptr_t node,
    kao2_eSRP *srpRef)
{
    kao2_eSRP *const localSRP = (kao2_eSRP *)
        (node + offsetof(kao2_eTransform, localSRP));

    *srpRef = *localSRP;
}

/**************************************************************/

void
kao2_eTransform_setSRP(
    uintptr_t node,
    kao2_eSRP *srpRef)
{
    kao2_eSRP *const localSRP = (kao2_eSRP *)
        (node + offsetof(kao2_eTransform, localSRP));

    *localSRP = *srpRef;
}

/**************************************************************/

bool
kao2_ePivot_getAnim(
    uintptr_t node,
    int *animIdRef,
    float *animTimerRef)
{
    /* Get `eAnimState` object */

    uintptr_t const animator = node + offsetof(kao2_ePivot, animate);

    uintptr_t const biList = *((uintptr_t *)
        (animator + offsetof(kao2_eAnimate, animStateBiListA)));

    if (0 == biList) { return false; }

    uintptr_t const animState = *((uintptr_t *) (biList + 0x08));

    if (0 == animState) { return false; }

    /* Calculate references to `eAnimState` fields */

    int32_t *const aID_ref = (int32_t *)
        (animState + offsetof(kao2_eAnimState, animId));

    float timer = *((float *)
        (animState + offsetof(kao2_eAnimState, timer)));

    float const startFrame = *((float *)
        (animState + offsetof(kao2_eAnimState, startFrame)));

    float const endFrame = *((float *)
        (animState + offsetof(kao2_eAnimState, endFrame)));

    /* Adjust timer for looping animations */

    float const framesLength = endFrame - startFrame;

    if (framesLength > 0)
    {
        timer = startFrame + _fmodf(timer, framesLength);
    }

    /* Read from `eAnimState` */

    *animIdRef    = *aID_ref;
    *animTimerRef = timer;

    return true;
}

/**************************************************************/

void
kao2_ePivot_setAnim(
    uintptr_t node,
    int animId,
    float animTimer)
{
    /* Get `eAnimState` object */

    uintptr_t const animator = node + offsetof(kao2_ePivot, animate);

    uintptr_t const biList = *((uintptr_t *)
        (animator + offsetof(kao2_eAnimate, animStateBiListA)));

    if (0 == biList) { return; }

    uintptr_t const animState = *((uintptr_t *) (biList + 0x08));

    if (0 == animState) { return; }

    /* Calculate references to `eAnimState` fields */

    int32_t *const aID_ref = (int32_t *)
        (animState + offsetof(kao2_eAnimState, animId));

    float *const timer_ref = (float *)
        (animState + offsetof(kao2_eAnimState, timer));

    float *const startFrame_ref = (float *)
        (animState + offsetof(kao2_eAnimState, startFrame));

    float *const endFrame_ref = (float *)
        (animState + offsetof(kao2_eAnimState, endFrame));

    /* Update `eAnimState` */

    *aID_ref = animId;

    *timer_ref      = animTimer;
    *startFrame_ref = animTimer;
    *endFrame_ref   = animTimer;
}

/**************************************************************/
