#include <Arduino.h>

byte writePin = 14;
byte readPin = 15;
byte clockPin = 16;
byte addressPins[16] = { 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52 };
byte dataPins[8] = { A8, A9, A10, A11, A12, A13, A14, A15 };

bool dataWrite = false;

unsigned int delayAfterWrite = 2000;

void setDataDirection(bool write) {
    if (dataWrite && !write) {
        for (int i = 0; i < 8; i++) {
            pinMode(dataPins[i], INPUT);
        }
        dataWrite = write;
    } else if (!dataWrite && write) {
        for (int i = 0; i < 8; i++) {
            pinMode(dataPins[i], OUTPUT);
        }
        dataWrite = write;
    }
}

void setAddress(unsigned short addr) {
    for (byte i = 0; i < 16; i++) {
        unsigned short b = 1 << i;
        if ((addr & b) > 0) {
            digitalWrite(addressPins[i], HIGH);
        } else {
            digitalWrite(addressPins[i], LOW);
        }
    }
}

byte readData() {
    byte result = 0;
    for (byte i = 0; i < 8; i++) {
        if (digitalRead(dataPins[i]) == HIGH) {
            result |= 1 << i;
        }
    }
    return result;
}

void writeData(byte value) {
    setDataDirection(true);

    // Serial.print("Writing: ");
    for (int i = 0; i < 8; i++) {
        if ((value & (1 << i)) == 0) {
            digitalWrite(dataPins[i], LOW);
            // Serial.print("0");
        } else {
            digitalWrite(dataPins[i], HIGH);
            // Serial.print("1");
        }
    }

    // Serial.print("(");
    // Serial.print(value);
    // Serial.println(")");

    digitalWrite(writePin, LOW);
    delayMicroseconds(delayAfterWrite);
    digitalWrite(writePin, HIGH);
    delayMicroseconds(delayAfterWrite);
}

void clockLow() {
    digitalWrite(clockPin, LOW);
    delayMicroseconds(100);
}

void clockHigh() {
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(100);
}

byte readByte(unsigned short address) {
    clockHigh();
    setDataDirection(false);
    digitalWrite(readPin, LOW);
    setAddress(address);

    clockLow();

    auto b = readData();

    digitalWrite(readPin, HIGH);

    return b;
}

void writeByte(unsigned short address, byte value) {
    clockHigh();
    setDataDirection(true);
    digitalWrite(writePin, LOW);
    setAddress(address);
    writeData(value);
    clockLow();
    digitalWrite(writePin, HIGH);
}

void readBytes(byte *buffer, unsigned short address, unsigned short count) {
    Serial.print("Reading | ");

    unsigned short inc = count / 32;

    unsigned long start = millis();
    for (unsigned short i = 0; i < count; i++) {
        buffer[i] = readByte(address + i);
        if (i % inc == 0) {
            Serial.print("#");
        }
    }
    unsigned long duration = millis() - start;

    Serial.print(" | ");
    Serial.print(duration);
    Serial.print("ms ");
    Serial.print(count / (duration + 0.0));
    Serial.print("bytes/ms");
    Serial.println();
}

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

void printBlock(unsigned short address, unsigned short bytes) {
    unsigned short addr = address;

    for (unsigned short j = 0; j < bytes / 16; j++) {
        printUShort(addr);
        Serial.print(": ");
        for (int i = 0; i < 16; i++) {
            auto d = readByte(addr);
            printByte(d);
            addr++;
        }
        Serial.println();
    }
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

    readBytes(a, address, count);
    delay(1000);
    readBytes(b, address, count);
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
    writeByte(0x2000, blockNumber & 0x00ff);
    writeByte(0x3000, (blockNumber & 0x0100) >> 8);
    Serial.print("Switched block to ");
    Serial.println(blockNumber);
}

void dumpMbc5() {
    Serial.println("Block 0");
    printBlock(0x0000, 0x0200);

    word block = 0x01;
    switchBlock(block);

    Serial.print("Block ");
    Serial.print(block);
    Serial.println();
    printBlock(0x4000, 0x0200);
}

void dumpData() {
    byte mbc = readByte(0x0147);
    Serial.print("MBC type = ");
    printByte(mbc);
    Serial.println();

    dumpMbc5();
}

void setupPins() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(writePin, OUTPUT);
    pinMode(readPin, OUTPUT);
    pinMode(clockPin, OUTPUT);

    for (int i = 0; i < 16; i++) {
        pinMode(addressPins[i], OUTPUT);
    }

    for (int i = 0; i < 8; i++) {
        pinMode(dataPins[i], INPUT);
    }
    dataWrite = false;

    digitalWrite(clockPin, LOW);
    digitalWrite(writePin, HIGH);
    digitalWrite(readPin, HIGH);
}

void setup() {
    setupPins();

    digitalWrite(LED_BUILTIN, HIGH);

    while (!Serial);
    Serial.begin(115200);

    readAndVerify(0x0000, 0x100);
    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
}