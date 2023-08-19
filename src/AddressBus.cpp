#include "AddressBus.h"

PinArrayAddressBus::PinArrayAddressBus(const uint8_t pins[16])
{
    memccpy(_addressPins, pins, 16, sizeof(uint8_t) * 16);
}

void PinArrayAddressBus::setAddress(const unsigned short address)
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

void PinArrayAddressBus::setupPins()
{
    for (int i = 0; i < 16; i++) {
        pinMode(_addressPins[i], OUTPUT);
    }
}

ShiftRegisterAddressBus::ShiftRegisterAddressBus(const uint8_t latchPin, const uint8_t clockPin, const uint8_t dataPin)
    : _latchPin(latchPin), _clockPin(clockPin), _dataPin(dataPin)
{
}

void ShiftRegisterAddressBus::setAddress(const unsigned short address)
{
    digitalWrite(_latchPin, LOW);

    shiftOut(_dataPin, _clockPin, LSBFIRST, address & 0xff);
    shiftOut(_dataPin, _clockPin, LSBFIRST, (address >> 8) & 0xff);

    digitalWrite(_latchPin, HIGH);
    delayMicroseconds(1000);
}

void ShiftRegisterAddressBus::setupPins(void)
{
    pinMode(_latchPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);
    digitalWrite(_latchPin, HIGH);
}
