/**
 * "COMMON/KAO2.H"
 */

#include "COMMON/CASSERT.H"

#include <stddef.h>
#include <stdint.h>

/**************************************************************/
/* Kao2 defines                                               */
/**************************************************************/

/* (18.11.2001, 13:51:59) */
#define KAO2_RETAIL_TIMESTAMP  0x3BF7BCFF

/* (29.09.2019, 21:07:36) */
#define KAO2_STEAMLESS_TIMESTAMP  0x5D911D18

/* (29.09.2019, 21:09:05) */
#define KAO2_DIGITAL_TIMESTAMP  0x5D911D71

/**************************************************************/

typedef struct kao2_ColorRGB kao2_ColorRGB;

struct kao2_ColorRGB
{
    float r, g, b;
};

/**************************************************************/

typedef struct kao2_eQuat kao2_eQuat;

struct kao2_eQuat
{
    /* [0x00] */ float x;
    /* [0x04] */ float y;
    /* [0x08] */ float z;
    /* [0x0C] */ float w;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eQuat, x));
STATIC_ASSERT(0x04 == offsetof(kao2_eQuat, y));
STATIC_ASSERT(0x08 == offsetof(kao2_eQuat, z));
STATIC_ASSERT(0x0C == offsetof(kao2_eQuat, w));

STATIC_ASSERT(0x10 == sizeof(kao2_eQuat));

/**************************************************************/

typedef struct kao2_ePoint3 kao2_ePoint3;

struct kao2_ePoint3
{
    /* [0x00] */ float x;
    /* [0x04] */ float y;
    /* [0x0C] */ float z;
};

STATIC_ASSERT(0x00 == offsetof(kao2_ePoint3, x));
STATIC_ASSERT(0x04 == offsetof(kao2_ePoint3, y));
STATIC_ASSERT(0x08 == offsetof(kao2_ePoint3, z));

STATIC_ASSERT(0x0C == sizeof(kao2_ePoint3));

/**************************************************************/

typedef struct kao2_eSRP kao2_eSRP;

struct kao2_eSRP
{
    /* [0x00] */ kao2_eQuat   rot;
    /* [0x10] */ kao2_ePoint3 pos;
    /* [0x1C] */ float        scale;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eSRP, rot));
STATIC_ASSERT(0x10 == offsetof(kao2_eSRP, pos));
STATIC_ASSERT(0x1C == offsetof(kao2_eSRP, scale));

STATIC_ASSERT(0x20 == sizeof(kao2_eSRP));

/**************************************************************/

typedef struct kao2_eAnimate kao2_eAnimate;

struct kao2_eAnimate
{
    /* [0x00] */ uintptr_t vft;
    /* [0x04] */ int32_t tracksCount;
    /* [0x08] */ int32_t tracksArraySize;
    /* [0x0C] */ uintptr_t tracks;
    /* [0x10] */ uintptr_t animStateBiListA;
    /* [0x14] */ uintptr_t animStateBiListB;
    /* [0x18] */ bool isPaused;
    /* [0x1C] */ float deltaTime;
    /* [0x20] */ float elapsedTime;
    /* [0x24] */ float rateBlendTo;
    /* [0x28] */ float rate;
    /* [0x2C] */ float totalElapsedTime;

    /* [0x30] */ struct { char pad[(0x3C - 0x30)]; } _pad1;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eAnimate, vft));
STATIC_ASSERT(0x04 == offsetof(kao2_eAnimate, tracksCount));
STATIC_ASSERT(0x08 == offsetof(kao2_eAnimate, tracksArraySize));
STATIC_ASSERT(0x0C == offsetof(kao2_eAnimate, tracks));
STATIC_ASSERT(0x10 == offsetof(kao2_eAnimate, animStateBiListA));
STATIC_ASSERT(0x14 == offsetof(kao2_eAnimate, animStateBiListB));
STATIC_ASSERT(0x18 == offsetof(kao2_eAnimate, isPaused));
STATIC_ASSERT(0x1C == offsetof(kao2_eAnimate, deltaTime));
STATIC_ASSERT(0x20 == offsetof(kao2_eAnimate, elapsedTime));
STATIC_ASSERT(0x24 == offsetof(kao2_eAnimate, rateBlendTo));
STATIC_ASSERT(0x28 == offsetof(kao2_eAnimate, rate));
STATIC_ASSERT(0x2C == offsetof(kao2_eAnimate, totalElapsedTime));

STATIC_ASSERT(0x3C == sizeof(kao2_eAnimate));

/**************************************************************/

typedef struct kao2_eStringHeader kao2_eStringHeader;

struct kao2_eStringHeader
{
    /* [0x00] */ int32_t refCount;
    /* [0x04] */ int32_t textLength;
    /* [0x08] */ int32_t arraySize;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eStringHeader, refCount));
STATIC_ASSERT(0x04 == offsetof(kao2_eStringHeader, textLength));
STATIC_ASSERT(0x08 == offsetof(kao2_eStringHeader, arraySize));

STATIC_ASSERT(0x0C == sizeof(kao2_eStringHeader));

/**************************************************************/

typedef struct kao2_eString kao2_eString;

struct kao2_eString
{
    /* [0x00] */ kao2_eStringHeader head;
    /* [0x0C] */ char text[1];
};

STATIC_ASSERT(0x00 == offsetof(kao2_eString, head));
STATIC_ASSERT(0x0C == offsetof(kao2_eString, text));

STATIC_ASSERT(0x10 == sizeof(kao2_eString));

STATIC_ASSERT(offsetof(kao2_eString, text) == sizeof(kao2_eStringHeader));

/**************************************************************/

typedef struct kao2_eObject kao2_eObject;

struct kao2_eObject
{
    /* [0x00] */ uintptr_t vft;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eObject, vft));

STATIC_ASSERT(0x04 == sizeof(kao2_eObject));

/**************************************************************/

typedef struct kao2_eRefCounter kao2_eRefCounter;

struct kao2_eRefCounter
{
    /* [0x00] */ kao2_eObject base;

    /* [0x04] */ int32_t refCount;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eRefCounter, base));
STATIC_ASSERT(0x04 == offsetof(kao2_eRefCounter, refCount));

STATIC_ASSERT(0x08 == sizeof(kao2_eRefCounter));

/**************************************************************/

typedef struct kao2_eAnimState kao2_eAnimState;

struct kao2_eAnimState
{
    /* [0x00] */ kao2_eRefCounter base;

    /* [0x08] */ float animId;
    /* [0x0C] */ int32_t dummy;
    /* [0x10] */ float timer;

    /* [0x14] */ struct { char pad[(0x34 - 0x14)]; } _pad1;

    /* [0x34] */ float startFrame;
    /* [0x38] */ float endFrame;
    /* [0x3C] */ int32_t loopType;

    /* [0x40] */ struct { char pad[(0x48 - 0x40)]; } _pad2;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eAnimState, base));
STATIC_ASSERT(0x08 == offsetof(kao2_eAnimState, animId));
STATIC_ASSERT(0x0C == offsetof(kao2_eAnimState, dummy));
STATIC_ASSERT(0x10 == offsetof(kao2_eAnimState, timer));
STATIC_ASSERT(0x34 == offsetof(kao2_eAnimState, startFrame));
STATIC_ASSERT(0x38 == offsetof(kao2_eAnimState, endFrame));
STATIC_ASSERT(0x3C == offsetof(kao2_eAnimState, loopType));

STATIC_ASSERT(0x48 == sizeof(kao2_eAnimState));

/**************************************************************/

typedef struct kao2_eNode kao2_eNode;

struct kao2_eNode
{
    /* [0x00] */ kao2_eRefCounter base;

    /* [0x08] */ uintptr_t lastXform;
    /* [0x0C] */ uint32_t dummyFlags;
    /* [0x10] */ uintptr_t parent;
    /* [0x14] */ uintptr_t name;
    /* [0x18] */ uintptr_t axisListBox;
    /* [0x1C] */ uint32_t visFlags;
    /* [0x20] */ float sphereBound[4];
    /* [0x30] */ uint16_t collisionFlags;
    /* [0x34] */ uintptr_t visCtrl;
    /* [0x38] */ float visRate;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eNode, base));
STATIC_ASSERT(0x08 == offsetof(kao2_eNode, lastXform));
STATIC_ASSERT(0x0C == offsetof(kao2_eNode, dummyFlags));
STATIC_ASSERT(0x10 == offsetof(kao2_eNode, parent));
STATIC_ASSERT(0x14 == offsetof(kao2_eNode, name));
STATIC_ASSERT(0x18 == offsetof(kao2_eNode, axisListBox));
STATIC_ASSERT(0x1C == offsetof(kao2_eNode, visFlags));
STATIC_ASSERT(0x20 == offsetof(kao2_eNode, sphereBound));
STATIC_ASSERT(0x30 == offsetof(kao2_eNode, collisionFlags));
STATIC_ASSERT(0x34 == offsetof(kao2_eNode, visCtrl));
STATIC_ASSERT(0x38 == offsetof(kao2_eNode, visRate));

STATIC_ASSERT(0x3C == sizeof(kao2_eNode));

/**************************************************************/

typedef struct kao2_eGeometry kao2_eGeometry;

struct kao2_eGeometry
{
    /* [0x00] */ kao2_eNode base;

    /* [0x3C] */ uintptr_t material;
    /* [0x40] */ kao2_ePoint3 boxBoundMin;
    /* [0x4C] */ kao2_ePoint3 boxBoundMax;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eGeometry, base));
STATIC_ASSERT(0x3C == offsetof(kao2_eGeometry, material));
STATIC_ASSERT(0x40 == offsetof(kao2_eGeometry, boxBoundMin));
STATIC_ASSERT(0x4C == offsetof(kao2_eGeometry, boxBoundMax));

STATIC_ASSERT(0x58 == sizeof(kao2_eGeometry));

/**************************************************************/

typedef struct kao2_eTextField kao2_eTextField;

struct kao2_eTextField
{
    /* [0x00] */ kao2_eGeometry base;

    /* [0x58] */ float scale;

    /* [0x5C] */ struct { char pad[(0xB0 - 0x5C)]; } _pad1;

    /* [0xB0] */ float x;
    /* [0xB4] */ float y;

    /* [0xB8] */ struct { char pad[(0xC0 - 0xB8)]; } _pad2;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eTextField, base));
STATIC_ASSERT(0x58 == offsetof(kao2_eTextField, scale));
STATIC_ASSERT(0xB0 == offsetof(kao2_eTextField, x));
STATIC_ASSERT(0xB4 == offsetof(kao2_eTextField, y));

STATIC_ASSERT(0xC0 == sizeof(kao2_eTextField));

/**************************************************************/

typedef struct kao2_eGroup kao2_eGroup;

struct kao2_eGroup
{
    /* [0x00] */ kao2_eNode base;

    /* [0x3C] */ int32_t count;
    /* [0x40] */ int32_t arraySize;
    /* [0x44] */ uintptr_t nodes;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eGroup, base));
STATIC_ASSERT(0x3C == offsetof(kao2_eGroup, count));
STATIC_ASSERT(0x40 == offsetof(kao2_eGroup, arraySize));
STATIC_ASSERT(0x44 == offsetof(kao2_eGroup, nodes));

STATIC_ASSERT(0x48 == sizeof(kao2_eGroup));

/**************************************************************/

typedef struct kao2_eTransform kao2_eTransform;

struct kao2_eTransform
{
    /* [0x00] */ kao2_eGroup base;

    /* [0x48] */ kao2_eSRP localSRP;
    /* [0x68] */ kao2_eSRP worldSRP;
    /* [0x88] */ kao2_eSRP tempSRP;
    /* [0xA8] */ uintptr_t animCtrl;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eTransform, base));
STATIC_ASSERT(0x48 == offsetof(kao2_eTransform, localSRP));
STATIC_ASSERT(0x68 == offsetof(kao2_eTransform, worldSRP));
STATIC_ASSERT(0x88 == offsetof(kao2_eTransform, tempSRP));
STATIC_ASSERT(0xA8 == offsetof(kao2_eTransform, animCtrl));

STATIC_ASSERT(0xAC == sizeof(kao2_eTransform));

/**************************************************************/

typedef struct kao2_eBillboard kao2_eBillboard;

struct kao2_eBillboard
{
    /* [0x0000] */ kao2_eTransform base;

    /* [0x00AC] */ bool enabled;
};

STATIC_ASSERT(0x00 == offsetof(kao2_eBillboard, base));
STATIC_ASSERT(0xAC == offsetof(kao2_eBillboard, enabled));

STATIC_ASSERT(0xB0 == sizeof(kao2_eBillboard));

/**************************************************************/

typedef struct kao2_ePivot kao2_ePivot;

struct kao2_ePivot
{
    /* [0x0000] */ kao2_eTransform base;

    /* [0x00AC] */ kao2_eAnimate animate;

    /* [0x00E8] */ struct { char pad[(0xF4 - 0xE8)]; } _pad1;

    /* [0x00F4] */ kao2_eSRP dummySRP;
};

STATIC_ASSERT(0x0000 == offsetof(kao2_ePivot, base));
STATIC_ASSERT(0x00AC == offsetof(kao2_ePivot, animate));
STATIC_ASSERT(0x00F4 == offsetof(kao2_ePivot, dummySRP));

STATIC_ASSERT(0x0114 == sizeof(kao2_ePivot));

/**************************************************************/

typedef struct kao2_Actor kao2_Actor;

struct kao2_Actor
{
    /* [0x0000] */ kao2_ePivot base;

    /* [0x0114] */ uintptr_t script;

    /* [0x0118] */ uintptr_t scriptPath;

    /* [0x011C] */ uint32_t dummy;
};

STATIC_ASSERT(0x0000 == offsetof(kao2_Actor, base));
STATIC_ASSERT(0x0114 == offsetof(kao2_Actor, script));
STATIC_ASSERT(0x0118 == offsetof(kao2_Actor, scriptPath));
STATIC_ASSERT(0x011C == offsetof(kao2_Actor, dummy));

STATIC_ASSERT(0x0120 == sizeof(kao2_Actor));

/**************************************************************/

typedef struct kao2_eKao2Gamelet kao2_eKao2Gamelet;

struct kao2_eKao2Gamelet
{
    /* [0x0000] */ kao2_eObject base;

    /* [0x0004] */ uintptr_t observer;
    /* [0x0008] */ uintptr_t drawContext;
    /* [0x000C] */ uintptr_t scene;
    /* [0x0010] */ float animSpeed;

    /* [0x0014] */ struct { char pad[(0x0360 - 0x0014)]; } _pad1;

    /* [0x0360] */ float sceneTime;

    /* [0x0364] */ struct { char pad[(0x038C - 0x0364)]; } _pad2;

    /* [0x038C] */ uintptr_t hero;
};

STATIC_ASSERT(0x0000 == offsetof(kao2_eKao2Gamelet, base));
STATIC_ASSERT(0x0004 == offsetof(kao2_eKao2Gamelet, observer));
STATIC_ASSERT(0x0008 == offsetof(kao2_eKao2Gamelet, drawContext));
STATIC_ASSERT(0x000C == offsetof(kao2_eKao2Gamelet, scene));
STATIC_ASSERT(0x0010 == offsetof(kao2_eKao2Gamelet, animSpeed));
STATIC_ASSERT(0x0360 == offsetof(kao2_eKao2Gamelet, sceneTime));
STATIC_ASSERT(0x038C == offsetof(kao2_eKao2Gamelet, hero));

/**************************************************************/
