#include "CartridgeReader.h"

CartridgeReader::CartridgeReader(const uint8_t addressPins[16], const uint8_t dataPins[8], const uint8_t writePin, const uint8_t readPin, const uint8_t clockPin)
{
    memccpy(_addressPins, addressPins, 16, sizeof(uint8_t) * 16);
    memccpy(_dataPins, dataPins, 8, sizeof(uint8_t) * 8);
    _writePin = writePin;
    _readPin = readPin;
    _clockPin = clockPin;
}

byte CartridgeReader::readByte(const unsigned short address)
{
    clockHigh();

    setDataToRead();
    digitalWrite(_readPin, LOW);
    setAddress(address);

    clockLow();

    auto b = readData();

    digitalWrite(_readPin, HIGH);

    return b;
}

void CartridgeReader::readBytes(byte *buffer, const unsigned short address, const unsigned short count)
{
    for (unsigned short i = 0; i < count; i++)
    {
        buffer[i] = readByte(address + i);
    }
}

void CartridgeReader::writeByte(const unsigned short address, const byte value)
{
    clockHigh();

    setDataToWrite();
    digitalWrite(_writePin, LOW);
    setAddress(address);
    writeData(value);

    clockLow();
    digitalWrite(_writePin, HIGH);
}

void CartridgeReader::setupPins(void)
{
    pinMode(_writePin, OUTPUT);
    pinMode(_readPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);

    for (int i = 0; i < 16; i++) {
        pinMode(_addressPins[i], OUTPUT);
    }

    for (int i = 0; i < 8; i++) {
        pinMode(_dataPins[i], INPUT);
    }

    digitalWrite(_clockPin, LOW);
    digitalWrite(_writePin, HIGH);
    digitalWrite(_writePin, HIGH);
}

void CartridgeReader::setDataToRead(void)
{
    if (!_dataWrite)
    {
        return;
    }

    for (int i = 0; i < 8; i++)
    {
        pinMode(_dataPins[i], INPUT);
    }
    _dataWrite = false;
}

void CartridgeReader::setDataToWrite(void)
{
    if (_dataWrite)
    {
        return;
    }

    for (int i = 0; i < 8; i++)
    {
        pinMode(_dataPins[i], OUTPUT);
    }
    _dataWrite = true;
}

void CartridgeReader::setAddress(const unsigned short address)
{
    for (byte i = 0; i < 16; i++)
    {
        unsigned short b = 1 << i;
        if ((address & b) > 0)
        {
            digitalWrite(_addressPins[i], HIGH);
        }
        else
        {
            digitalWrite(_addressPins[i], LOW);
        }
    }
}

void CartridgeReader::clockHigh(void)
{
    digitalWrite(_clockPin, HIGH);
    delayMicroseconds(ClockToggleDelayMicroseconds);
}

void CartridgeReader::clockLow(void)
{
    digitalWrite(_clockPin, LOW);
    delayMicroseconds(ClockToggleDelayMicroseconds);
}

byte CartridgeReader::readData(void)
{
    byte result = 0;
    for (byte i = 0; i < 8; i++)
    {
        if (digitalRead(_dataPins[i]) == HIGH)
        {
            result |= 1 << i;
        }
    }
    return result;
}

void CartridgeReader::writeData(byte value)
{
    for (int i = 0; i < 8; i++)
    {
        if ((value & (1 << i)) == 0)
        {
            digitalWrite(_dataPins[i], LOW);
        }
        else
        {
            digitalWrite(_dataPins[i], HIGH);
        }
    }
}
