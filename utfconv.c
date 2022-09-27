#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define MASK_FIRST_BYTE 0xC0
#define MASK_NEXT_BYTES 0x80

bool hasUTF8(const uint8_t *utf8_buff, uint16_t utf8_buff_len)
{
    bool ret = false;
    if (utf8_buff != NULL)
    {
        for (int i = 0; i < utf8_buff_len; i++)
        {
            if ((utf8_buff[i] & MASK_FIRST_BYTE) == MASK_FIRST_BYTE)
            {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

const uint8_t *getCodeFromUTF8(const uint8_t *utf8_buff, uint16_t utf8_buff_len, uint32_t *code)
{
    uint32_t temp = 0;
    int8_t size = 0;
    const int8_t *retAddr = NULL;

    if ((utf8_buff != NULL) && (code != NULL) && (utf8_buff_len != 0))
    {
        for (uint8_t i = 0; i < utf8_buff_len; i++)
        {
            uint8_t mask = MASK_FIRST_BYTE;
            while ((utf8_buff[i] & mask) == mask)
            {
                mask = (int8_t)mask >> 1;
                size++;
            }

            if (mask != MASK_FIRST_BYTE)
            {
                temp = (utf8_buff[i] & ~mask);
            }
            else if ((utf8_buff[i] & MASK_NEXT_BYTES) == MASK_NEXT_BYTES)
            {
                temp = (temp << 6) | (utf8_buff[i] & 0x3F);
                size--;
            }
            else
            {
                temp = utf8_buff[i];
                size = 0;
            }

            if (size == 0)
            {
                printf("%x %x %d %x\n", utf8_buff[i], mask, size, temp);
                *code = temp;
                retAddr = (utf8_buff + i + 1);
                break;
            }
        }
    }
    return retAddr;
}

void convUTF16(const uint8_t *utf8_buff, uint16_t utf8_buff_len, uint16_t *utf16_buff)
{
    const uint8_t *begin = utf8_buff;
    const uint8_t *end = &utf8_buff[utf8_buff_len];
    uint16_t idx = 0;
    while (begin != end)
    {
        uint32_t code = 0;
        begin = getCodeFromUTF8(begin, (end - begin), &code);
        if (begin == NULL)
        {
            printf("error while getting code point!\n");
            break;
        }
        else
        {
            if (code > 0xFFFF)
            {
                code = code - 0x10000;
                utf16_buff[idx++] = (code >> 10) + 0xD800;
                utf16_buff[idx++] = (code & 0x3FF) + 0xDC00;
            }
            else
            {
                utf16_buff[idx++] = (uint16_t)code;
            }
        }
    }
}

int main()
{
    // bYҁ짼幎娳򰇑9ޫފؓ󀀪䝶1�򊆈镶䉆𡹑োÉƯ󲎱ς؅趏e܀撄𴟇懗
    uint8_t utf8[] = {0x62, 0x59, 0xd2, 0x81, 0xec, 0xa7, 0xbc, 0xe5, 0xb9, 0x8e, 0xe5, 0xa8, 0xb3, 0xf2, 0xb0, 0x87, 0x91, 0xee, 0x82, 0x8a, 0x39, 0xde, 0xab, 0xde, 0x8a, 0xd8, 0x93, 0xf3, 0x80, 0x80, 0xaa, 0xe4, 0x9d, 0xb6, 0x31, 0xef, 0xbf, 0xbd, 0xf2, 0x8a, 0x86, 0x88, 0xe9, 0x95, 0xb6, 0xe4, 0x89, 0x86, 0xf0, 0xa1, 0xb9, 0x91, 0xe0, 0xa7, 0x8b, 0xc3, 0x89, 0xc6, 0xaf, 0xf3, 0xb2, 0x8e, 0xb1, 0xcf, 0x82, 0xd8, 0x85, 0xe8, 0xb6, 0x8f, 0x65, 0xdc, 0x80, 0xe6, 0x92, 0x84, 0xf0, 0xb4, 0x9f, 0x87, 0xe6, 0x87, 0x97};
    uint16_t utf16[256] = {0x00};
    const uint8_t *utf16_p = (uint8_t *)utf16;

    if (hasUTF8(utf8, sizeof(utf8)))
    {
        convUTF16(utf8, sizeof(utf8), utf16);
    }

    printf("\n");
    for (int i = 0; i < sizeof(utf16); i++)
    {
        printf("0x%x, ", utf16_p[i]);
    }
    printf("\n");

    return 0;
}
