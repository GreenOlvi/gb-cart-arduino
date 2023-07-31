#include "InputHelper.h"

void skipSpaces(const char *input, int &i)
{
    while (input[i] == ' ')
    {
        i++;
    }
}

bool tryConsumeCommand(const char *input, int &i, char &cmd)
{
    switch (input[i])
    {
    case 'r':
        cmd = 'r';
        i++;
        return true;

    case 'w':
        cmd = 'w';
        i++;
        return true;

    default:
        return false;
    }
}

bool tryParseHex(const char* input, int &i, unsigned short &value)
{
    unsigned short tmp = 0;
    auto j = i + 2;

    while ((input[j] >= '0' && input[j] <= '9') || (input[j] >= 'a' && input[j] <= 'f'))
    {
        tmp *= 16;
        char c = input[j];
        if (c >= '0' && c <= '9')
        {
            tmp += c - '0';
        }
        else
        {
            tmp += 10 + c - 'a';
        }

        j++;
    }

    i = j;
    value = tmp;
    return true;
}

bool tryParseDec(const char *input, int &i, unsigned short &value)
{
    unsigned short tmp = 0;
    auto j = i;

    while (input[j] >= '0' && input[j] <= '9')
    {
        tmp = tmp * 10 + (input[j] - '0');
        j++;
    }

    i = j;
    value = tmp;
    return true;
}

bool tryConsumeNumber(const char *input, int &i, unsigned short &value)
{
    if (input[i] == '0' && input[i + 1] == 'x')
    {
        return tryParseHex(input, i, value);
    }
    else
    {
        return tryParseDec(input, i, value);
    }
}

CommandType parseRead(const char *input, int &i, unsigned short &arg1, unsigned short &arg2)
{
    skipSpaces(input, i);

    unsigned short address;
    if (!tryConsumeNumber(input, i, address))
    {
        return CommandType::Unknown;
    }

#if PARSER_DEBUG
    Serial.print("Address = ");
    Serial.println(address);
#endif

    if (input[i] == 0x00)
    {
        arg1 = address;
        return CommandType::ReadByte;
    }

    if (input[i] == '-')
    {
        i++;
        unsigned short endAddress;
        if (!tryParseHex(input, i, endAddress))
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

    if (input[i] == ' ')
    {
        skipSpaces(input, i);

        unsigned short count;
        if (!tryParseHex(input, i, count))
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
    skipSpaces(input, i);

    unsigned short address;
    if (!tryConsumeNumber(input, i, address))
    {
        return CommandType::Unknown;
    }

#if PARSER_DEBUG
    Serial.print("Address = ");
    Serial.println(address);
#endif

    if (input[i] == ' ')
    {
        skipSpaces(input, i);

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

    skipSpaces(input, i);

    char cmd;
    if (!tryConsumeCommand(input, i, cmd))
    {
        return CommandType::Unknown;
    }

#if PARSER_DEBUG
    Serial.print("Command char = ");
    Serial.println(cmd);
#endif

    switch (cmd) {
        case 'r':
            return parseRead(input, i, arg1, arg2);
        case 'w':
            return parseWrite(input, i, arg1, arg2);
        default:
            return CommandType::Unknown;
    }
}