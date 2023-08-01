#ifndef InputHelper_h
#define InputHelper_h

#include <Arduino.h>

#define PARSER_DEBUG 0

enum CommandType
{
    Unknown = 0,
    ReadByte,
    ReadBlockWithCount,
    ReadBlockAbsolute,
    WriteByte,
};

CommandType parseInput(const char *input, unsigned short &arg1, unsigned short &arg2);

bool tryParseNumber(const char *input, int &i, unsigned short &value);
bool tryParseDec(const char* input, int &i, unsigned short &value);
bool tryParseHex(const char* input, int &i, unsigned short &value);

#endif