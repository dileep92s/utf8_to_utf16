#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define MASK_FIRST_BYTE 0xC0
#define MASK_NEXT_BYTES 0x80

bool hasUTF8(const uint8_t *data, uint16_t len)
{
    bool ret = false;
    if (data != NULL)
    {
        for (int i = 0; i < len; i++)
        {
            if ((data[i] & MASK_FIRST_BYTE) == MASK_FIRST_BYTE)
            {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

const uint8_t *getUTF16(const uint8_t *data, uint16_t len, uint16_t *utf16)
{
    uint32_t temp = 0;
    int8_t size = 0;
    const int8_t *ret = NULL;

    if ((data != NULL) && (utf16 != NULL) && (len != 0))
    {
        *utf16 = 0;
        for (uint8_t i = 0; i < len; i++)
        {
            uint8_t mask = MASK_FIRST_BYTE;
            while ((data[i] & mask) == mask)
            {
                mask = (int8_t)mask >> 1;
                size++;
            }

            if (mask != MASK_FIRST_BYTE)
            {
                temp = (data[i] & ~mask);
            }
            else if ((data[i] & MASK_NEXT_BYTES) == MASK_NEXT_BYTES)
            {
                temp = (temp << 6) | (data[i] & 0x3F);
                size--;
            }
            else
            {
                temp = data[i];
                size = 0;
            }

            if (size == 0)
            {
                printf("%x %x %d %x\n", data[i], mask, size, temp);
                ret = (data + i + 1);
                break;
            }
        }
    }
    return ret;
}

uint16_t convUTF16(const uint8_t *utf8, uint16_t utf8_len, uint16_t *utf16)
{
    uint16_t utf16_len = 0;
    uint16_t utf16_char = 0;
    const uint8_t *begin = utf8;
    const uint8_t *end = &utf8[utf8_len];
    while (begin != end)
    {
        uint16_t len = end - begin;
        begin = getUTF16(begin, len, &utf16_char);
        if (begin == NULL)
        {
            printf("parse error\n");
            break;
        }
    }
    return utf16_len;
}

int main()
{
    // uint8_t data[] = {0xe2, 0x82, 0xac};
    // uint8_t data[] = {0xF0, 0x90, 0x8D, 0x88};
    uint8_t utf8[] = {0xF0, 0x90, 0x8D, 0x88, 0xe2, 0x82, 0xac, 0x65};
    uint16_t utf16[16] = {0x00};

    if (hasUTF8(utf8, sizeof(utf8)))
    {
        convUTF16(utf8, sizeof(utf8), utf16);
    }

    // convUTF16(utf8, sizeof(utf8), NULL);

    return 0;
}