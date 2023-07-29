#ifndef CartridgeReader_h
#define CartridgeReader_h

#include <Arduino.h>

#define ClockToggleDelayMicroseconds 10

class Cartridge
{
public:
    Cartridge(const uint8_t addressPins[16], const uint8_t dataPins[8], const uint8_t writePin, const uint8_t readPin, const uint8_t clockPin);

    byte readByte(const unsigned short address);
    void readBytes(byte *buffer, const unsigned short address, const unsigned short count);

    void writeByte(const unsigned short address, const byte value);

private:
    uint8_t _addressPins[16];
    uint8_t _dataPins[8];
    uint8_t _writePin;
    uint8_t _readPin;
    uint8_t _clockPin;

    bool _dataWrite = false;

    void setupPins(void);

    void setDataToRead(void);
    void setDataToWrite(void);
    void setAddress(const unsigned short address);

    void clockHigh(void);
    void clockLow(void);

    byte readData(void);
    void writeData(const byte value);
};

#endif