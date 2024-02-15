/**
 * "KAO2PLUS/GAME2.CPP"
 */

#include "KAO2PLUS.HPP"

/**************************************************************/

char gTextBuffer[KAO2PLUS__STATUS_MSG__LENGTH + 1];

/**************************************************************/

char const *
kao2_Game_WStrToCStr(
    WCHAR const *utf16Text)
{
    char *buffer = gTextBuffer;

    while (*utf16Text)
    {
        WCHAR x = (*utf16Text);
        if (x >= 128) { x = L' '; }
        (*buffer) = (char) x;

        buffer++;
        utf16Text++;
    }

    (*buffer) = '\0';

    return gTextBuffer;
}

/**************************************************************/

int
kao2_Game_PokeMemory(
    uintptr_t lpAddress,
    void const *lpcSource,
    DWORD dwSize)
{
    BOOL bTest;
    DWORD flOldProtect;
    DWORD flNewProtect;

    bTest = VirtualProtect(
        lpAddress,
        dwSize,
        PAGE_EXECUTE_READWRITE,
        &(flOldProtect));

    if (FALSE == bTest) { return 1; }

    memcpy((void *) lpAddress, lpcSource, dwSize);

    bTest = VirtualProtect(
        lpAddress,
        dwSize,
        flOldProtect,
        &(flNewProtect));

    if (FALSE == bTest) { return 1; }

    return 0;
}

/**************************************************************/

bool
kao2_eNode_Cloning_Restore(
    uintptr_t node,
    uintptr_t *buffer)
{
    if (node != buffer[0]) { return false; }

    /* Restore type? */

    if (0 != buffer[1])
    {
        *((uintptr_t *) node) = buffer[1];
    }

    /* Restore `eALBox`? */

    if (0 != buffer[2])
    {
        uintptr_t *const albRef = (uintptr_t *)
            (node + offsetof(kao2_eNode, axisListBox));

        *albRef = buffer[2];
    }

    /* Buffer used - advance the pointer */

    return true;
}

/**************************************************************/

bool
kao2_eNode_Cloning_StoreAndHide(
    uintptr_t node,
    uintptr_t *buffer)
{
    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const pivotVFT = moduleBase + (gContext->vft_ePivot);

    uintptr_t const pivotType = moduleBase + (gContext->typeInfo_ePivot);

    bool storedType  = false;
    bool storedALBox = false;

    /* Is derived from `ePivot`? */

    uintptr_t const objectType = kao2_eObject_getTypeInfo(node);

    if (objectType != pivotType)
    {
        if (kao2_TypeInfo_isDerivedFrom(objectType, pivotType))
        {
            /* Degrade down to `ePivot` (animations without scripts) */

            buffer[1] = *((uintptr_t *) node);

            *((uintptr_t *) node) = pivotVFT;

            storedType = true;
        }
    }

    /* Has non-empty `eALBox`? */

    uintptr_t *const albRef = (uintptr_t *)
        (node + offsetof(kao2_eNode, axisListBox));

    if (0 != (*albRef))
    {
        buffer[2] = *albRef;

        *albRef = 0;

        storedALBox = true;
    }

    /* Finish hiding */

    if (storedType or storedALBox)
    {
        buffer[0] = node;

        if (not storedType)  { buffer[1] = 0; }
        if (not storedALBox) { buffer[2] = 0; }

        /* Buffer used - advance the pointer */

        return true;
    }

    return false;
}

/**************************************************************/

bool
kao2_eNode_Cloning_Count(
    uintptr_t node)
{
    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const pivotType = moduleBase + (gContext->typeInfo_ePivot);

    uintptr_t const objectType = kao2_eObject_getTypeInfo(node);

    /* Is derived from `ePivot`? */

    if (objectType != pivotType)
    {
        if (kao2_TypeInfo_isDerivedFrom(objectType, pivotType))
        {
            return true;
        }
    }

    /* Has non-empty `eALBox`? */

    uintptr_t *const albRef = (uintptr_t *)
        (node + offsetof(kao2_eNode, axisListBox));

    if (0 != (*albRef)) { return true; }

    return false;
}

/**************************************************************/

void
kao2_eGroup_Cloning_Restore(
    uintptr_t node,
    uintptr_t **buffer)
{
    if (kao2_eNode_Cloning_Restore(node, (*buffer)))
    {
        (*buffer) += 3;
    }

    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const groupType  = moduleBase + (gContext->typeInfo_eGroup);

    uintptr_t const objectType = kao2_eObject_getTypeInfo(node);

    if (kao2_TypeInfo_isDerivedFrom(objectType, groupType))
    {
        int const count = *((int *)
            (node + offsetof(kao2_eGroup, count)));

        uintptr_t *const array = *((uintptr_t **)
            (node + offsetof(kao2_eGroup, nodes)));

        for (int i = 0; i < count; i++)
        {
            kao2_eGroup_Cloning_Restore(array[i], buffer);
        }
    }
}

/**************************************************************/

void
kao2_eGroup_Cloning_StoreAndHide(
    uintptr_t node,
    uintptr_t **buffer)
{
    if (kao2_eNode_Cloning_StoreAndHide(node, (*buffer)))
    {
        (*buffer) += 3;
    }

    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const groupType  = moduleBase + (gContext->typeInfo_eGroup);

    uintptr_t const objectType = kao2_eObject_getTypeInfo(node);

    if (kao2_TypeInfo_isDerivedFrom(objectType, groupType))
    {
        int const count = *((int *)
            (node + offsetof(kao2_eGroup, count)));

        uintptr_t *const array = *((uintptr_t **)
            (node + offsetof(kao2_eGroup, nodes)));

        for (int i = 0; i < count; i++)
        {
            kao2_eGroup_Cloning_StoreAndHide(array[i], buffer);
        }
    }
}

/**************************************************************/

int
kao2_eGroup_Cloning_Count(
    uintptr_t node)
{
    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const groupType  = moduleBase + (gContext->typeInfo_eGroup);

    uintptr_t const objectType = kao2_eObject_getTypeInfo(node);

    /* Count objects that need to be temporarily changed */

    int sum = 0;

    if (kao2_eNode_Cloning_Count(node)) { sum = 1; }

    if (kao2_TypeInfo_isDerivedFrom(objectType, groupType))
    {
        int const count = *((int *)
            (node + offsetof(kao2_eGroup, count)));

        uintptr_t *const array = *((uintptr_t **)
            (node + offsetof(kao2_eGroup, nodes)));

        for (int i = 0; i < count; i++)
        {
            sum += kao2_eGroup_Cloning_Count(array[i]);
        }
    }

    return sum;
}

/**************************************************************/

void
kao2_Game_CloneHero_CleanUp(
    uintptr_t newHero)
{
    uintptr_t const moduleBase = GetModuleHandle();

    uintptr_t const groupType = moduleBase + (gContext->typeInfo_eGroup);
    uintptr_t const xformType = moduleBase + (gContext->typeInfo_eTransform);
    uintptr_t const meshType  = moduleBase + (gContext->typeInfo_eTriMesh);

    /* Find and remove `shadowCaster.mesh` */

    uintptr_t const shadowCasterMesh =
        kao2_eGroup_findExact(
            newHero,
            meshType,
            "Kao-shadow / main");

    if (0 != shadowCasterMesh)
    {
        kao2_eNode_detachFromParent(shadowCasterMesh);
    }

    /**
     * Find and remove any instances of `eGroup`
     * "MultiTargetThrower group", which contains either:
     * - `WaterBullet` ("water_bullet")
     * - `PowerGlove` ("boomerang_bullet")
     */
    while (1)
    {
        uintptr_t const multiTargetThrowerBullets =
            kao2_eGroup_findExact(
                newHero,
                groupType,
                "MultiTargetThrower group");

        if (0 == multiTargetThrowerBullets) { break; }

        kao2_eNode_detachFromParent(multiTargetThrowerBullets);
    }

    /**
     * Find `eTransform` "szyja_rot" and remove its `eExtraPosMultiCtrl`
     * (nullify the effects of `ePadLookAtCtrl` and `eGiraffeNeck`)
     */
    uintptr_t const neckRotXform =
        kao2_eGroup_findExact(newHero, xformType, "szyja_rot");

    if (0 != neckRotXform)
    {
        uintptr_t *extraPostMultiCtrl = (uintptr_t *)
            (neckRotXform + offsetof(kao2_eTransform, animCtrl));

        kao2_eRefCounter_decRef(*extraPostMultiCtrl);

        *extraPostMultiCtrl = 0;
    }
}

/**************************************************************/

__declspec(noinline)
uintptr_t
kao2_Game_CloneHero(void)
{
    uintptr_t const moduleBase = GetModuleHandle();

    /* Load game procedures */

    CloneFunc const cloneFunc = (CloneFunc)
        (moduleBase + (gContext->global_clone));

    MallocFunc const mallocFunc = kao2_Game_GetMallocFunc();
    FreeFunc const freeFunc = kao2_Game_GetFreeFunc();

    /* Get `Actor` "$hero" address */

    uintptr_t const hero = kao2_Game_GetHero();

    if (0 == hero) { return 0; }

    /* Temporarily change node tree */

    int const numChangedNodes = kao2_eGroup_Cloning_Count(hero);

    uintptr_t* bufChangedNodes = (uintptr_t *)
        mallocFunc(3 * sizeof(uintptr_t) * numChangedNodes);

    uintptr_t* bufChangedNodes2 = bufChangedNodes;
    kao2_eGroup_Cloning_StoreAndHide(hero, &(bufChangedNodes2));

    /**
     * Temporarily replace "children nodes" count with 1
     * (leaving only `eEnvironment` "global_env",
     * getting rid of `eListener` and other script-nodes)
     */
    int32_t *const heroChildCount = (int32_t *)
        (hero + offsetof(kao2_eGroup, count));

    int32_t const prevChildCount = *heroChildCount;
    *heroChildCount = 1;

    /**
     * Treat "$hero" as `ePivot` and call the `clone()` method
     *
     * Arg `FALSE` results in "no clone in eMultiCtrl<eSRPCtrl>" error.
     */
    int32_t cloner[4] = { TRUE, 0, 0, 0 };

    uintptr_t newHero = cloneFunc(
        /* (Cloner  *) cloner */ (uintptr_t) &(cloner),
        /* (eObject *) object */ hero);

    /* Clean-up the `Cloner` */

    if (0 != cloner[3]) { freeFunc(cloner[3]); }

    /* Clean-up the "$hero" */

    *heroChildCount = prevChildCount;

    /* Restore properties in node tree */

    bufChangedNodes2 = bufChangedNodes;
    kao2_eGroup_Cloning_Restore(hero, &(bufChangedNodes2));

    freeFunc((uintptr_t) bufChangedNodes);

    /* Check if the hero clone is valid */

    if (0 == newHero) { return 0; }

    kao2_Game_CloneHero_CleanUp(newHero);

    return newHero;
}

/**************************************************************/

bool
kao2_Game_CreateNicknameTextField(
    uintptr_t heroNode,
    uintptr_t *textFieldRef,
    size_t capacity)
{
    /* Check if the object is no longer valid and remove it */

    uintptr_t textField = *textFieldRef;

    if (0 != textField)
    {
        if (1 == kao2_eRefCounter_getRef(textField))
        {
            kao2_eRefCounter_decRef(textField);

            *textFieldRef = 0;
            textField = 0;
        }
        else
        {
            return true;
        }
    }

    /**
     * Creating new object hierachy:
     * { hero clone => billboard => xform => textField }
     *
     * Game will automatically run its error handler on failed `malloc`
     */

    uintptr_t const moduleBase = GetModuleHandle();

    MallocFunc const mallocFunc = kao2_Game_GetMallocFunc();

    TextFieldCtorFunc const textFieldCtor = (TextFieldCtorFunc)
        (moduleBase + (gContext->textField_ctor));

    uintptr_t const boardType = moduleBase + (gContext->typeInfo_eBillboard);
    uintptr_t const xformType = moduleBase + (gContext->typeInfo_eTransform);

    /* Create `eBillboard` node */

    uintptr_t const board = kao2_TypeInfo_newObject(boardType);

    bool *const boardEnabled = (bool *)
        (board + offsetof(kao2_eBillboard, enabled));

    *boardEnabled = true;

    /**
     * Create `eTransform` node
     *
     * rotation: { X = -90, Y = 0, Z = 0 }
     * position: over hero's head
     */

    uintptr_t const xform = kao2_TypeInfo_newObject(xformType);

    kao2_eSRP *const srp = (kao2_eSRP *)
        (xform + offsetof(kao2_eTransform, localSRP));

    srp->rot.x = 0.707107f;
    srp->rot.y = 0;
    srp->rot.z = 0;
    srp->rot.w = 0.707107f;

    srp->pos.x = 0;
    srp->pos.y = 0;
    srp->pos.z = 350.f;

    /**
     * Create `eTextField` node and update it
     *
     * text scale: 75%
     */

    textField = mallocFunc(sizeof(kao2_eTextField));

    textFieldCtor(textField, capacity);

    float *const scale = (float *)
        (textField + offsetof(kao2_eTextField, scale));

    *scale = 0.75f;

    /* Load virtual procedures */

    uintptr_t const heroVFT  = *((uintptr_t *) heroNode);
    uintptr_t const boardVFT = *((uintptr_t *) board);
    uintptr_t const xformVFT = *((uintptr_t *) xform);

    AppendNodeFunc const appendToHero = (AppendNodeFunc)
        *((uintptr_t *) (heroVFT + 0x70));

    AppendNodeFunc const appendToBoard = (AppendNodeFunc)
        *((uintptr_t *) (boardVFT + 0x70));

    AppendNodeFunc const appendToXform = (AppendNodeFunc)
        *((uintptr_t *) (xformVFT + 0x70));

    /* Set up the node hierachy */

    appendToXform(xform, textField, (-1));
    appendToBoard(board, xform, (-1));
    appendToHero(heroNode, board, 0);

    *textFieldRef = textField;

    return (2 == kao2_eRefCounter_incRef(textField));
}

/**************************************************************/

void
kao2_Game_UpdateNicknameTextField(
    uintptr_t textField,
    kao2_ColorRGB const *color,
    char const *text)
{
    uintptr_t const moduleBase = GetModuleHandle();

    TextFieldSetColorFunc const setColor = (TextFieldSetColorFunc)
        (moduleBase + (gContext->textField_setColor));

    TextFieldSetTextFunc const setText = (TextFieldSetTextFunc)
        (moduleBase + (gContext->textField_setText));

    /**
     * Update `eTextField` node
     *
     * offset X: centered text (more-or-less, good approx)
     */

    float *const xOffset = (float *)
        (textField + offsetof(kao2_eTextField, x));

    *xOffset = strlen(text) * 0.5f * (- 25.f);

    /* Update color and show the actual text */

    setColor(textField, color->r, color->g, color->b, 1.0f, (-1));

    setText(textField, text, false);
}

/**************************************************************/

bool
kao2_Game_CreateStatusMsgTextField(
    uintptr_t parentNode,
    uintptr_t *textFieldRef,
    int32_t index)
{
    /* Check if the object is no longer valid and remove it */

    uintptr_t textField = *textFieldRef;

    if (0 != textField)
    {
        if (1 == kao2_eRefCounter_getRef(textField))
        {
            kao2_eRefCounter_decRef(textField);

            *textFieldRef = 0;
            textField = 0;
        }
        else
        {
            return true;
        }
    }

    /* Load game procedures */

    uintptr_t const moduleBase = GetModuleHandle();

    MallocFunc const mallocFunc = kao2_Game_GetMallocFunc();

    TextFieldCtorFunc const textFieldCtor = (TextFieldCtorFunc)
        (moduleBase + (gContext->textField_ctor));

    /**
     * Create `eTextField` node and update it
     *
     * text scale: 25%
     * Y offset: higher message index => lower placement
     */

    textField = mallocFunc(sizeof(kao2_eTextField));

    textFieldCtor(textField, KAO2PLUS__STATUS_MSG__LENGTH);

    float *const scaleRef = (float *)
        (textField + offsetof(kao2_eTextField, scale));

    float *const yOffsetRef = (float *)
        (textField + offsetof(kao2_eTextField, y));

    float const textScale = 0.25f;
    float const lineHeight = (textScale * 1.5f * 25.f);

    *scaleRef = textScale;
    *yOffsetRef = lineHeight * (0.5f + index * 1.5f);

    /* Load virtual procedures */

    uintptr_t const parentVFT  = *((uintptr_t *) parentNode);

    AppendNodeFunc const appendToParent = (AppendNodeFunc)
        *((uintptr_t *) (parentVFT + 0x70));

    /* Set up the node hierachy */

    appendToParent(parentNode, textField, (-1));

    *textFieldRef = textField;

    return (2 == kao2_eRefCounter_incRef(textField));
}

/**************************************************************/

void
kao2_Game_UpdateStatusMsgTextField(
    uintptr_t textField,
    char const *text,
    float alpha)
{
    uintptr_t const moduleBase = GetModuleHandle();

    TextFieldSetColorFunc const setColor = (TextFieldSetColorFunc)
        (moduleBase + (gContext->textField_setColor));

    TextFieldSetTextFunc const setText = (TextFieldSetTextFunc)
        (moduleBase + (gContext->textField_setText));

    /* Update `eTextField` text */

    setText(textField, text, false);

    /* Update `eTextField` visibility */

    float *const visAlpha = (float *)
        (textField + offsetof(kao2_eNode, visRate));

    if (alpha > 0)
    {
        *visAlpha = 1.f;
        setColor(textField, 1.0f, 1.0f, 1.0f, alpha, (-1));
    }
    else
    {
        *visAlpha = 0;
    }
}

/**************************************************************/
