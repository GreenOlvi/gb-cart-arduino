#ifndef OutputHelper_h
#define OutputHelper_h

#include <Arduino.h>
#include <Base64.h>

void hexDumpByte(const byte b, const unsigned short address = 0x0000);
void hexDumpByteArray(const byte *arr, const unsigned short count, const unsigned short address = 0x0000);

bool cmpArrays(const byte *a, const byte *b, const unsigned short count);

void encodeBase64(const byte b);
void encodeBase64(const byte *arr, const unsigned short count);

#endif