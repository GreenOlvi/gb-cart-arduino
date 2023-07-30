#include <Arduino.h>
#include "Cartridge.h"
#include "InputHelper.h"

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

void dumpData()
{
    byte mbc = reader.readByte(0x0147);
    Serial.print("MBC type = ");
    printByte(mbc);
    Serial.println();
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

    // initTests();

    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("> ");
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
            Serial.print(rc);
        }
        else {
            inputChars[inputIndex] = '\0';
            inputIndex = 0;
            inputFinished = true;
            Serial.println();
        }
    }

    return inputFinished;
}

void readBlockCommand(const unsigned short address, const unsigned short count)
{
    byte *a = new byte[count];
    reader.readBytes(a, address, count);
    dumpByteArray(a, count, address);
}

void printByteCommand(const unsigned short address)
{
    auto b = reader.readByte(address);
    printUShort(address);
    Serial.print(": ");
    printByte(b);
    Serial.println();
    Serial.println();
}

void runCommand(const CommandType command, const unsigned short arg1, const unsigned short arg2)
{
    switch (command)
    {
    case CommandType::ReadByte:
        printByteCommand(arg1);
        break;

    case CommandType::ReadBlockWithCount:
        readBlockCommand(arg1, arg2);
        break;
    
    case CommandType::ReadBlockAbsolute:
        readBlockCommand(arg1, arg2 - arg1);
        break;

    case CommandType::WriteByte:
        reader.writeByte(arg1, arg2);
        printByteCommand(arg1);
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
        Serial.print("Read string: [");
        Serial.print(inputChars);
        Serial.println("]");

        unsigned short arg1, arg2;
        CommandType c = parseInput(inputChars, arg1, arg2);
        runCommand(c, arg1, arg2);
    }
}