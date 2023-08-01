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

CommandType parseRead(const char *input, int &i, unsigned short &arg1, unsigned short &arg2)
{
    tryConsumeSpaces(input, i);

    unsigned short address;
    if (!tryParseNumber(input, i, address))
    {
        return CommandType::Unknown;
    }

#if PARSER_DEBUG
    Serial.print("Address = ");
    Serial.println(address);
#endif

    if (isFinished(input, i))
    {
        arg1 = address;
        return CommandType::ReadByte;
    }

    if (tryConsumeOne(input, i, '-'))
    {
        unsigned short endAddress;
        if (!tryParseNumber(input, i, endAddress))
        {
            return CommandType::Unknown;
        }

#if PARSER_DEBUG
        Serial.print("End address = ");
        Serial.println(endAddress);
#endif

        arg1 = address;
        arg2 = endAddress;
        return CommandType::ReadBlockAbsolute;
    }

    if (tryConsumeSpaces(input, i))
    {
        unsigned short count;
        if (!tryParseNumber(input, i, count))
        {
            return CommandType::Unknown;
        }

#if PARSER_DEBUG
        Serial.print("Count = ");
        Serial.println(count);
#endif

        arg1 = address;
        arg2 = count;
        return CommandType::ReadBlockWithCount;
    }

    return CommandType::Unknown;
}

CommandType parseWrite(const char *input, int &i, unsigned short &arg1, unsigned short &arg2)
{
    tryConsumeSpaces(input, i);

    unsigned short address;
    if (!tryParseNumber(input, i, address))
    {
        return CommandType::Unknown;
    }

#if PARSER_DEBUG
    Serial.print("Address = ");
    Serial.println(address);
#endif

    if (tryConsumeSpaces(input, i))
    {
        unsigned short value;
        if (!tryParseHex(input, i, value))
        {
            return CommandType::Unknown;
        }

#if PARSER_DEBUG
        Serial.print("Value = ");
        Serial.println(value);
#endif

        arg1 = address;
        arg2 = value;
        return CommandType::WriteByte;
    }

    return CommandType::Unknown;
}

CommandType parseInput(const char *input, unsigned short &arg1, unsigned short &arg2)
{
    int i = 0;

    tryConsumeSpaces(input, i);

    if (tryConsumeOne(input, i, 'r'))
    {
        return parseRead(input, i, arg1, arg2);
    }

    if (tryConsumeOne(input, i, 'w'))
    {
        return parseWrite(input, i, arg1, arg2);
    }

    return CommandType::Unknown;
}