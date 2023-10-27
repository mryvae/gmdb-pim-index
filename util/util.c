#include "util.h"

uint32_t align8(uint32_t s)
{
    if ((s & 0x7) == 0)
        return s;
    return ((s >> 3) + 1) << 3;
}

uint32_t next_power(uint32_t size)
{
    uint32_t i = 1;

    if (size >= 2147483648U)
        return 2147483648U;
    while (1)
    {
        if (i >= size)
            return i;
        i *= 2;
    }
}

char *itoa_t(int num, char *str)
{
    int num_len = 0;
    char temp[16];
    while (num)
    {
        temp[num_len] = (char)(num % 10 + '0');
        num /= 10;
        num_len++;
    }

    for (int i = 0; i < num_len; i++)
    {
        str[i] = temp[num_len - i - 1];
    }
    str[num_len] = '\0';
    return str;
}