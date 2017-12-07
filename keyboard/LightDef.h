#ifndef LIGHT_DEF_H
#define LIGHT_DEF_H

#if defined (CT_322)
enum ELightValue
{
    LIGHT_CHROMA = 8,
    LIGHT_FREQ = 9,
    LIGHT_AUTO = 10,
    LIGHT_FREEZE = 11,
    LIGHT_UPDOWN = 14,
    LIGHT_ZOOM = 13,
    LIGHT_LEFTRIGHT = 12,
    LIGHT_FOCUS = 15,
    LIGHT_CALC = 16,
    LIGHT_MEASURE = 17,
    LIGHT_CURSOR = 18,
    LIGHT_SAVE = 19,
    LIGHT_READ = 20,
    LIGHT_BM = 21,
    LIGHT_M = 22,
    LIGHT_BB = 23,
    LIGHT_D2 = 24,
    LIGHT_4B = 25,
    LIGHT_TSI = 26,
    LIGHT_BODYMARK = 27,
    LIGHT_CAPSLOCK = 28,
    LIGHT_SYSTEM = 29,
    LIGHT_DEPTH = 30, //reserve
};
#elif defined (CT_313)
enum ELightValue
{
    LIGHT_ARROW = 8,
    LIGHT_FREQ = 15,
    LIGHT_CURSOR = 11,
    LIGHT_ZOOM = 9,
    LIGHT_FOCUS = 14,
    LIGHT_UPDOWN = 19,
    LIGHT_LEFTRIGHT = 16,
    LIGHT_CALC = 18,
    LIGHT_READ = 17,
    LIGHT_BM = 0,
    LIGHT_M = 1,
    LIGHT_BB = 2,
    LIGHT_D2 = 3,
    LIGHT_DEPTH = 10,
    LIGHT_FREEZE = 20,
    LIGHT_AUTO = 22,
    LIGHT_4B = 25,
    LIGHT_TSI = 26,
    LIGHT_BODYMARK = 27,
    LIGHT_CAPSLOCK = 28,
    LIGHT_SAVE = 19,
    LIGHT_SYSTEM = 29,
};

#elif (defined(CT_460) || defined(CT_355))
enum ELightValue
{
#if 0
    LIGHT_VALUE = 22, //not used now
#endif
    // add new
    LIGHT_BODYMARK = 0x08,
    LIGHT_QUAD = 0x09,
    LIGHT_INVERT = 0x0A,
    LIGHT_LEFTRIGHT = 0xB,
    LIGHT_UPDOWN = 0x0C,
    LIGHT_CW = 0x0D,
    LIGHT_PDI = 0x0E,
    LIGHT_PW = 0x0F,
    LIGHT_CFM = 0x11,
    LIGHT_D2 = 0x12,
    LIGHT_DUAL = 0x13,
    LIGHT_M = 0x14,
    LIGHT_FREEZE = 0x15,
    LIGHT_FOCUS = 0x17,
    LIGHT_ZOOM = 0x19,
    LIGHT_POINTER = 0x1A,
    LIGHT_MEASURE = 0x1B,

    LIGHT_MENU = 11,
    LIGHT_AUTO = 13,
};

#else
enum ELightValue
{
    LIGHT_CAPSLOCK = 8, //not used now
    LIGHT_SHIFT = 9, //not used now
    LIGHT_UPDOWN = 10, //not used now
    LIGHT_MENU = 11,
    LIGHT_FREEZE = 12,
    LIGHT_SAVE = 13, //not used now
    LIGHT_P1 = 14, //not used now
    LIGHT_P2 = 15, //not used now
    LIGHT_M = 16,
    LIGHT_CW = 17,
    LIGHT_PDI = 18,
    LIGHT_PW = 19,
    LIGHT_CFM = 20,
    LIGHT_D2 = 21,
    LIGHT_AUTO = 22,
    LIGHT_LEFTRIGHT = 23, //not used now
};
#endif
#endif