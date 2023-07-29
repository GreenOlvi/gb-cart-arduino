#include <Arduino.h>
#include "Cartridge.h"

#define WRITE_PIN 14
#define READ_PIN 15
#define CLOCK_PIN 16
byte addressPins[16] = { 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52 };
byte dataPins[8] = { A8, A9, A10, A11, A12, A13, A14, A15 };

Cartridge reader(addressPins, dataPins, WRITE_PIN, READ_PIN, CLOCK_PIN);

void printByte(byte b) {
    char str[4];
    sprintf(str, " %02x", b);
    Serial.print(str);
}

void printUShort(unsigned short s) {
    char str[6];
    sprintf(str, " %04x", s);
    Serial.print(str);
}

bool arraysEqual(byte *a, byte *b, unsigned short count) {
    for (unsigned short i = 0; i < count; i++) {
        if (a[i] != b[i]) {
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

void dumpByteArray(byte *arr, unsigned short count, unsigned short address = 0x0000) {
    for (unsigned short j = 0; j < count / 16; j++) {
        printUShort((j * 16) + address);
        Serial.print(": ");
        for (int i = 0; i < 16; i++) {
            printByte(arr[(j << 4) + i]);
        }
        Serial.println();
    }
}

bool readAndVerify(unsigned short address, unsigned short count) {
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

    if (result) {
        Serial.println("Success");
    } else {
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

void switchBlock(word blockNumber) {
    reader.writeByte(0x2000, blockNumber & 0x00ff);
    reader.writeByte(0x3000, (blockNumber & 0x0100) >> 8);
    Serial.print("Switched to block ");
    Serial.println(blockNumber);
}

void dumpData() {
    byte mbc = reader.readByte(0x0147);
    Serial.print("MBC type = ");
    printByte(mbc);
    Serial.println();
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    while (!Serial);
    Serial.begin(115200);

    Serial.println("Reading first block");
    readAndVerify(0x0000, 0x400);

    Serial.println("Switching blocks");
    switchBlock(0x0000);
    readAndVerify(0x4000, 0x200);
    switchBlock(0x0001);
    readAndVerify(0x4000, 0x200);

    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
}