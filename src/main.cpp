#include <Arduino.h>
#include <Base64.h>
#include "Cartridge.h"
#include "InputHelper.h"

#define VERSION "1.0-alpha"

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

void printByte(byte b)
{
    char str[4];
    sprintf(str, " %02x", b);
    Serial.print(str);
}

void printUShort(unsigned short s)
{
    char str[6];
    sprintf(str, " %04x", s);
    Serial.print(str);
}

bool arraysEqual(byte *a, byte *b, unsigned short count)
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

void dumpByteArray(byte *arr, unsigned short count, unsigned short address = 0x0000)
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

    bool result = arraysEqual(a, b, count);

    if (result)
    {
        Serial.println("Success");
    }
    else
    {
        Serial.println("Failure");
        Serial.println("First:");
        dumpByteArray(a, count, address);
        Serial.println("Second:");
    }

    dumpByteArray(b, count, address);

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
    dumpByteArray(a, count, address);
#endif
#if ENCODED_OUTPUT
    int encodedLength = Base64.encodedLength(count);
    char encodedString[encodedLength + 1];
    Base64.encode(encodedString, (char *)a, count);
    Serial.println(encodedString);
#endif
}

void printByteCommandResult(const unsigned short address)
{
    auto b = reader.readByte(address);
#if READABLE_OUTPUT
    printUShort(address);
    Serial.print(": ");
    printByte(b);
    Serial.println();
#endif
#if ENCODED_OUTPUT
    char encodedString[5];
    Base64.encode(encodedString, (char *)&b, 1);
    Serial.println(encodedString);
#endif
}

void runCommand(const CommandType command, const unsigned short arg1, const unsigned short arg2)
{
    switch (command)
    {
    case CommandType::ReadByte:
        printByteCommandResult(arg1);
        break;

    case CommandType::ReadBlockWithCount:
        readBlockCommandResult(arg1, arg2);
        break;
    
    case CommandType::ReadBlockAbsolute:
        readBlockCommandResult(arg1, arg2 - arg1 + 1);
        break;

    case CommandType::WriteByte:
        reader.writeByte(arg1, arg2);
        printByteCommandResult(arg1);
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