#include "Cartridge.h"

Cartridge::Cartridge(const uint8_t addressPins[16], const uint8_t dataPins[8], const uint8_t writePin, const uint8_t readPin, const uint8_t clockPin)
    : _writePin(writePin), _readPin(readPin), _clockPin(clockPin)
{
    _addressBus = new PinArrayAddressBus(addressPins);
    memccpy(_dataPins, dataPins, 8, sizeof(uint8_t) * 8);
    setupPins();
}

Cartridge::Cartridge(const uint8_t addrLatchPin, const uint8_t addrClockPin, const uint8_t addrDataPin, const uint8_t dataPins[8], const uint8_t writePin, const uint8_t readPin, const uint8_t clockPin)
    : _writePin(writePin), _readPin(readPin), _clockPin(clockPin)
{
    _addressBus = new ShiftRegisterAddressBus(addrLatchPin, addrClockPin, addrDataPin);
    memccpy(_dataPins, dataPins, 8, sizeof(uint8_t) * 8);
    setupPins();
}

byte Cartridge::readByte(const unsigned short address)
{
    clockHigh();

    setDataToRead();
    digitalWrite(_readPin, LOW);
    _addressBus->setAddress(address);

    clockLow();

    auto b = readData();

    digitalWrite(_readPin, HIGH);

    return b;
}

void Cartridge::readBytes(byte *buffer, const unsigned short address, const unsigned short count)
{
    for (unsigned short i = 0; i < count; i++)
    {
        buffer[i] = readByte(address + i);
    }
}

void Cartridge::writeByte(const unsigned short address, const byte value)
{
    clockHigh();

    setDataToWrite();
    digitalWrite(_writePin, LOW);
    _addressBus->setAddress(address);
    writeData(value);

    clockLow();
    digitalWrite(_writePin, HIGH);
}

void Cartridge::setupPins(void)
{
    pinMode(_writePin, OUTPUT);
    pinMode(_readPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);

    for (int i = 0; i < 8; i++) {
        pinMode(_dataPins[i], INPUT);
    }

    _addressBus->setupPins();

    digitalWrite(_clockPin, LOW);
    digitalWrite(_writePin, HIGH);
    digitalWrite(_writePin, HIGH);
}

void Cartridge::setDataToRead(void)
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

void Cartridge::setDataToWrite(void)
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

void Cartridge::clockHigh(void)
{
    digitalWrite(_clockPin, HIGH);
    delayMicroseconds(ClockToggleDelayMicroseconds);
}

void Cartridge::clockLow(void)
{
    digitalWrite(_clockPin, LOW);
    delayMicroseconds(ClockToggleDelayMicroseconds);
}

byte Cartridge::readData(void)
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

void Cartridge::writeData(byte value)
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
