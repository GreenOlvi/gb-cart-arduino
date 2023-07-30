#ifndef InputHelper_h
#define InputHelper_h

#include <Arduino.h>

enum CommandType
{
    Unknown = 0,
    ReadByte,
    ReadBlockWithCount,
    ReadBlockAbsolute,
    WriteByte,
};

bool readInput();
CommandType parseInput(const char *input, unsigned short &arg1, unsigned short &arg2);

#endif