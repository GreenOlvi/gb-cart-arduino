#ifndef PIO_UNIT_TESTING

#include <Arduino.h>
#include <Base64.h>
#include "Cartridge.h"
#include "InputHelper.h"
#include "OutputHelper.h"

#define VERSION "1.0.0"

#define INIT_TESTS 0
#define ENCODED_OUTPUT 1
#define READABLE_OUTPUT 0
#define INPUT_ECHO 1

#define WRITE_PIN 14
#define READ_PIN 15
#define CLOCK_PIN 16
byte addressPins[16] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
byte dataPins[8] = {A8, A9, A10, A11, A12, A13, A14, A15};

Cartridge reader(addressPins, dataPins, WRITE_PIN, READ_PIN, CLOCK_PIN);

bool readAndVerify(unsigned short address, unsigned short count)
{
    byte *a = new byte[count];
    byte *b = new byte[count];

    auto firstStart = millis();
    Serial.print("Reading first... ");
    reader.readBytes(a, address, count);
    auto firstTime = millis() - firstStart;
    Serial.print(firstTime);
    Serial.println("ms");

    delay(1000);

    auto secondStart = millis();
    Serial.print("Reading second... ");
    reader.readBytes(b, address, count);
    auto secondTime = millis() - secondStart;
    Serial.print(secondTime);
    Serial.println("ms");

    bool result = cmpArrays(a, b, count);

    if (result)
    {
        Serial.println("Success");
    }
    else
    {
        Serial.println("Failure");
        Serial.println("First:");
        hexDumpByteArray(a, count, address);
        Serial.println("Second:");
    }

    hexDumpByteArray(b, count, address);

    free(a);
    free(b);

    return result;
}

void switchBlock(word blockNumber)
{
    reader.writeByte(0x2000, blockNumber & 0x00ff);
    reader.writeByte(0x3000, (blockNumber & 0x0100) >> 8);
    Serial.print("Switched to block ");
    Serial.println(blockNumber);
}

void initTests()
{
    Serial.println("Reading first block");
    readAndVerify(0x0000, 0x400);

    Serial.println("Switching blocks");
    switchBlock(0x0000);
    readAndVerify(0x4000, 0x200);
    switchBlock(0x0001);
    readAndVerify(0x4000, 0x200);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    while (!Serial);
    Serial.begin(115200);

    Serial.println("GB cart reader " VERSION);

#if INIT_TESTS
    initTests();
#endif

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println();
}

const char endChar = '\n';
const byte maxChars = 32;
char inputChars[maxChars];
byte inputIndex = 0;

bool readInput()
{
    char rc;
    bool inputFinished = false;

    if (Serial.available() > 0) {
        rc = Serial.read();

        if (rc != endChar) {
            inputChars[inputIndex] = rc;
            inputIndex++;
            if (inputIndex >= maxChars)
            {
                inputIndex = maxChars - 1;
            }
            #if INPUT_ECHO
            Serial.print(rc);
            #endif
        }
        else {
            inputChars[inputIndex] = '\0';
            inputIndex = 0;
            inputFinished = true;
            #if INPUT_ECHO
            Serial.println();
            #endif
        }
    }

    return inputFinished;
}

void readBlockCommandResult(const unsigned short address, const unsigned short count)
{
    byte a[count];
    reader.readBytes(a, address, count);
#if READABLE_OUTPUT
    hexDumpByteArray(a, count, address);
#endif
#if ENCODED_OUTPUT
    encodeBase64(a, count);
#endif
}

void readByteCommandResult(const unsigned short address)
{
    auto b = reader.readByte(address);
#if READABLE_OUTPUT
    hexDumpByte(b, address);
#endif
#if ENCODED_OUTPUT
    encodeBase64(b);
#endif
}

void runCommand(const CommandType command, const unsigned short arg1, const unsigned short arg2)
{
    switch (command)
    {
    case CommandType::ReadByte:
        readByteCommandResult(arg1);
        break;

    case CommandType::ReadBlockWithCount:
        readBlockCommandResult(arg1, arg2);
        break;
    
    case CommandType::ReadBlockAbsolute:
        readBlockCommandResult(arg1, arg2 - arg1 + 1);
        break;

    case CommandType::WriteByte:
        reader.writeByte(arg1, arg2);
        readByteCommandResult(arg1);
        break;

    default:
        Serial.println("Unknown command");
        break;
    }
}

void loop()
{
    if (readInput())
    {
        unsigned short arg1, arg2;
        CommandType c = parseInput(inputChars, arg1, arg2);
        runCommand(c, arg1, arg2);
    }
}

#endif