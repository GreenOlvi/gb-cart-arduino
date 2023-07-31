#include "OutputHelper.h"

void printByte(const byte b)
{
    char str[4];
    sprintf(str, " %02x", b);
    Serial.print(str);
}

void printUShort(const unsigned short s)
{
    char str[6];
    sprintf(str, " %04x", s);
    Serial.print(str);
}

void hexDumpByte(const byte b, const unsigned short address)
{
    printUShort(address);
    Serial.print(": ");
    printByte(b);
    Serial.println();
}

void hexDumpByteArray(const byte *arr, const unsigned short count, const unsigned short address)
{
    for (unsigned short j = 0; j < count / 16; j++)
    {
        printUShort((j * 16) + address);
        Serial.print(": ");
        for (int i = 0; i < 16; i++)
        {
            printByte(arr[(j << 4) + i]);
        }
        Serial.println();
    }

    int r = count % 16;
    if (r > 0)
    {
        int j = count - r;
        printUShort(j + address);
        Serial.print(": ");
        for (int i = 0; i < r; i++)
        {
            printByte(arr[j + i]);
        }
        Serial.println();
    }
}

bool cmpArrays(const byte *a, const byte *b, const unsigned short count)
{
    for (unsigned short i = 0; i < count; i++)
    {
        if (a[i] != b[i])
        {
            Serial.print("Difference at 0x");
            Serial.println(i, 16);
            printByte(a[i]);
            printByte(b[i]);
            Serial.println();
            return false;
        }
    }
    return true;
}

void encodeBase64(const byte b)
{
    char encodedString[5];
    Base64.encode(encodedString, (char *)&b, 1);
    Serial.println(encodedString);
}

void encodeBase64(const byte *arr, const unsigned short count)
{
    int encodedLength = Base64.encodedLength(count);
    char encodedString[encodedLength + 1];
    Base64.encode(encodedString, (char *)arr, count);
    Serial.println(encodedString);
}
