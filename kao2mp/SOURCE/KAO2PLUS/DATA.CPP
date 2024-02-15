/**
 * "KAO2PLUS/DATA.CPP"
 */

#include "KAO2PLUS.HPP"

/**************************************************************/

unsigned _fltused;

kao2_GameContext const *gContext;
uint32_t gGameVersion;

/**************************************************************/

kao2_GameContext const gKao2_Retail_Context =
{
    .global_glutDisplayCallback = 0x000902D0,
    .global_mallocWrapper       = 0x00061F40,
    .global_freeWrapper         = 0x00061FC0,
    .global_clone               = 0x00065410,

    .textField_ctor     = 0x00149BB0,
    .textField_setColor = 0x0014A130,
    .textField_setText  = 0x00149F90,

    .gamelet    = 0x002267CC,
    .quitFlag   = 0x002267DC,
    .dinputKeyb = 0x00226EB0,
    .levelList  = 0x0022B7C8,

    .typeInfo_eNode      = 0x002245C0,
    .typeInfo_eGroup     = 0x00224890,
    .typeInfo_eTransform = 0x00224748,
    .typeInfo_eBillboard = 0x00229424,
    .typeInfo_ePivot     = 0x002248EC,
    .typeInfo_eTriMesh   = 0x00224428,

    .vft_eGroup = 0x001D0F88,
    .vft_ePivot = 0x001D1140,

    .gamelet_orthoNode = 0x30C4,
};

/**************************************************************/

kao2_GameContext const gKao2_Steamless_Context =
{
    .global_glutDisplayCallback = 0x00176EE0,
    .global_mallocWrapper       = 0x001C4C20,
    .global_freeWrapper         = 0x001C4BF0,
    .global_clone               = 0x001C6D80,

    .textField_ctor     = 0x000B0DA0,
    .textField_setColor = 0x000B1370,
    .textField_setText  = 0x000B1220,

    .gamelet    = 0x0073E0F8,
    .quitFlag   = 0x0073E104,
    .dinputKeyb = 0x0073BAC8,
    .levelList  = 0x00734C98,

    .typeInfo_eNode      = 0x0073B60C,
    .typeInfo_eGroup     = 0x0073B9F4,
    .typeInfo_eTransform = 0x0073B7B4,
    .typeInfo_eBillboard = 0x00746DA8,
    .typeInfo_ePivot     = 0x0073BA4C,
    .typeInfo_eTriMesh   = 0x0073B500,

    .vft_eGroup = 0x005836BC,
    .vft_ePivot = 0x00583C7C,

    .gamelet_orthoNode = 0x3768,
};

/**************************************************************/

kao2_GameContext const gKao2_Digital_Context =
{
    .global_glutDisplayCallback = 0x00176EE0,
    .global_mallocWrapper       = 0x001C3B80,
    .global_freeWrapper         = 0x001C3B50,
    .global_clone               = 0x01C5CE0,

    .textField_ctor     = 0x000B0DA0,
    .textField_setColor = 0x000B1370,
    .textField_setText  = 0x000B1220,

    .gamelet    = 0x0073CEF8,
    .quitFlag   = 0x0073CF04,
    .dinputKeyb = 0x0073A8C8,
    .levelList  = 0x00733A98,

    .typeInfo_eNode      = 0x0073A40C,
    .typeInfo_eGroup     = 0x0073A7F4,
    .typeInfo_eTransform = 0x0073A5B4,
    .typeInfo_eBillboard = 0x00745BA8,
    .typeInfo_ePivot     = 0x0073A84C,
    .typeInfo_eTriMesh   = 0x0073A300,

    .vft_eGroup = 0x0058269C,
    .vft_ePivot = 0x00582C5C,

    .gamelet_orthoNode = 0x3768,
};

/**************************************************************/
