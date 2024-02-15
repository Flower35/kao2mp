/**
 * "KAO2PLUS/HELPERS.CPP"
 */

#include "KAO2PLUS.HPP"

/**************************************************************/

#pragma function(memcpy)
void *
memcpy(
    void *dest,
    void const *src,
    size_t n)
{
    char *destBytes = (char *) dest;
    char const *srcBytes = (char const *) src;

    while (n--)
    {
        *(destBytes++) = *(srcBytes++);
    }

    return dest;
}

/**************************************************************/

#pragma function(memcmp)
int
memcmp(
    void const *mem1,
    void const *mem2,
    size_t n)
{
    char const *s1 = (char const *) mem1;
    char const *s2 = (char const *) mem2;

    for (; n--; s1++, s2++)
    {
        if ((*s1) != (*s2)) { return (*s1) - (*s2); }
    }

    return 0;
}

/**************************************************************/

char *
__cdecl
strncpy(
    char *dest,
    char const *src,
    size_t n)
{
    while (n--)
    {
        if (*src)
        {
            *dest = *src;

            dest++;
            src++;
        }
        else
        {
            *dest = '\0';
            return dest;
        }
    }

    return dest;
}

/**************************************************************/

long
__cdecl
_ftol(
    double x)
{
    long result;

    __asm
    {
        fld     x
        fistp   result
        mov     eax, result
    }
}

/**************************************************************/

float
__cdecl
_fmodf(
    float x,
    float y)
{
    float dummy;
    float remainder;

    /**
     * "fprem": replace ST(0) with the remainder
     * obtained from dividing ST(0) by ST(1).
     */

    /**
     * | fxch    st0, st1 |
     * | fprem            |
     * | fwait            |
     * | fstp    st1, st0 |
     */
    __asm
    {
        fld     y
        fld     x
        fprem
        fwait
        fstp    remainder
        fstp    dummy
    }

    return remainder;
}

/**************************************************************/

char *
__cdecl
_strdup(
    char const *str)
{
    size_t const len = strlen(str);
    char *result = (char *) kao2_Game_GetMallocFunc() (1 + len);

    memcpy(result, str, len);
    result[len] = '\0';

    return result;
}

/**************************************************************/

void
Dummy_UInt16ToTextDec(
    char *buf,
    uint16_t value)
{
    /* (2^16 - 1) == 65535 */

    buf[0] = '0';
    buf[1] = '0';
    buf[2] = '0';
    buf[3] = '0';
    buf[4] = '0';

    buf += 5;

    buf[0] = '\0';

    buf--;

    while (0 != value)
    {
        int digit = value % 10;
        value = value / 10;

        *buf = (char) ('0' + digit);
        buf--;
    }
}

/**************************************************************/

void
Dummy_UInt32ToText(
    char *buf,
    uintptr_t value)
{
    buf[0] = '0';
    buf[1] = 'x';

    buf += 2;

    buf[0] = '0';
    buf[1] = '0';
    buf[2] = '0';
    buf[3] = '0';
    buf[4] = '0';
    buf[5] = '0';
    buf[6] = '0';
    buf[7] = '0';

    buf += 8;

    buf[0] = '\0';

    buf--;

    while (0 != value)
    {
        int digit = value % 16;
        value = value / 16;

        if ((digit >= 0) and (digit <= 9))
        {
            *buf = (char) ('0' + digit);
        }
        else
        {
            *buf = (char) ('a' + (digit - 10));
        }

        buf--;
    }
}

/**************************************************************/

int32_t
Dummy_TextToInt32(
    char const *buf)
{
    bool sign = false;
    int32_t value = 0;

    if ((*buf) and ('-' == (*buf)))
    {
        sign = true;
        buf++;
    }

    while (*buf)
    {
        if (((*buf) >= '0') and ((*buf) <= '9'))
        {
            value = 10 * value + ((*buf) - '0');
        }
        else
        {
            return 0;
        }

        buf++;
    }

    return sign ? (- value) : value;
}

/**************************************************************/

uintptr_t
GetModuleHandle(void)
{
    __asm
    {
        mov     eax, dword ptr fs:[0x30]
        mov     eax, dword ptr [eax + 8]
    }
}

/**************************************************************/
