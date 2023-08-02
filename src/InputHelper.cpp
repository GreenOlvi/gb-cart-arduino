#include "InputHelper.h"

bool isFinished(const char *input, const int &i)
{
    return input[i] == 0x00;
}

bool tryConsumeOne(const char *input, int &i, char c)
{
    if (input[i] != c)
    {
        return false;
    }

    i++;
    return true;
}

bool tryConsumeOneOrMany(const char *input, int &i, char c)
{
    if (input[i] != c)
    {
        return false;
    }

    while (input[i] == c)
    {
        i++;
    }

    return true;
}

bool tryConsumeSpaces(const char *input, int &i)
{
    return tryConsumeOneOrMany(input, i, ' ');
}

bool isHexDigit(const char c)
{
    return (c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F');
}

bool isDecDigit(const char c)
{
    return (c >= '0' && c <= '9');
}

bool tryParseHex(const char* input, int &i, unsigned short &value)
{
    if (input[i] != '0' || input[i + 1] != 'x' || !isHexDigit(input[i + 2]))
    {
        return false;
    }

    unsigned short tmp = 0;
    auto j = i + 2;

    while (isHexDigit(input[j]))
    {
        tmp *= 16;
        char c = input[j];
        if (c >= '0' && c <= '9')
        {
            tmp += c - '0';
        }
        else
        {
            tmp += 10 + toLowerCase(c) - 'a';
        }

        j++;
    }

    i = j;
    value = tmp;
    return true;
}

bool tryParseDec(const char *input, int &i, unsigned short &value)
{
    if (!isDecDigit(input[i]))
    {
        return false;
    }

    unsigned short tmp = 0;
    auto j = i;

    while (isDecDigit(input[j]))
    {
        tmp = tmp * 10 + (input[j] - '0');
        j++;
    }

    i = j;
    value = tmp;
    return true;
}

bool tryParseNumber(const char *input, int &i, unsigned short &value)
{
    return tryParseHex(input, i, value)
        || tryParseDec(input, i, value);
}

bool tryParseReadByteCommand(const char *input, unsigned short &arg1)
{
    int i = 0;
    unsigned short address;
    if (tryConsumeOne(input, i, 'r')
        && tryConsumeSpaces(input, i)
        && tryParseNumber(input, i, address)
        && isFinished(input, i))
    {

        #if PARSER_DEBUG
        Serial.print("ReadByte(Address=");
        Serial.print(address);
        Serial.println(")");
        #endif

        arg1 = address;
        return true;
    }

    return false;
}

bool tryParseReadBlockWithCountCommand(const char *input, unsigned short &arg1, unsigned short &arg2)
{
    int i = 0;
    unsigned short address, count;
    if (tryConsumeOne(input, i, 'r')
        && tryConsumeSpaces(input, i)
        && tryParseNumber(input, i, address)
        && tryConsumeSpaces(input, i)
        && tryParseNumber(input, i, count))
    {

        #if PARSER_DEBUG
        Serial.print("ReadBlockWithCount(Address=");
        Serial.print(address);
        Serial.print(";Count=");
        Serial.print(count);
        Serial.println(")");
        #endif

        arg1 = address;
        arg2 = count;
        return true;
    }

    return false;
}

bool tryParseReadBlockAbsoluteCommand(const char *input, unsigned short &arg1, unsigned short &arg2)
{
    int i = 0;
    unsigned short addressStart, addressEnd;
    if (tryConsumeOne(input, i, 'r')
        && tryConsumeSpaces(input, i)
        && tryParseNumber(input, i, addressStart)
        && tryConsumeOne(input, i, '-')
        && tryParseNumber(input, i, addressEnd))
    {

        #if PARSER_DEBUG
        Serial.print("ReadBlockAbsolute(AddressStart=");
        Serial.print(addressStart);
        Serial.print(";AddressEnd=");
        Serial.print(addressEnd);
        Serial.println(")");
        #endif

        arg1 = addressStart;
        arg2 = addressEnd;
        return true;
    }

    return false;
}

bool tryParseWriteByteCommand(const char *input, unsigned short &arg1, unsigned short &arg2)
{
    int i = 0;
    unsigned short address, value;
    if (tryConsumeOne(input, i, 'w')
        && tryConsumeSpaces(input, i)
        && tryParseNumber(input, i, address)
        && tryConsumeSpaces(input, i)
        && tryParseNumber(input, i, value))
    {

        #if PARSER_DEBUG
        Serial.print("ReadBlockAbsolute(Address=");
        Serial.print(address);
        Serial.print(";Value=");
        Serial.print(value);
        Serial.println(")");
        #endif

        arg1 = address;
        arg2 = value;
        return true;
    }

    return false;
}

CommandType parseInput(const char *input, unsigned short &arg1, unsigned short &arg2)
{
    if (tryParseReadByteCommand(input, arg1))
    {
        return CommandType::ReadByte;
    }

    if (tryParseReadBlockWithCountCommand(input, arg1, arg2))
    {
        return CommandType::ReadBlockWithCount;
    }

    if (tryParseReadBlockAbsoluteCommand(input, arg1, arg2))
    {
        return CommandType::ReadBlockAbsolute;
    }

    if (tryParseWriteByteCommand(input, arg1, arg2))
    {
        return CommandType::WriteByte;
    }

    return CommandType::Unknown;
}