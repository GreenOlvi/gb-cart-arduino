#ifndef AddressBus_h
#define AddressBus_h

#include <Arduino.h>

class AddressBus
{
public:
    virtual void setAddress(const unsigned short address) = 0;
    virtual void setupPins(void) = 0;
};

class PinArrayAddressBus : public AddressBus
{
public:
    PinArrayAddressBus(const uint8_t pins[16]);
    void setAddress(const unsigned short address) override;
    void setupPins(void);

private:
    uint8_t _addressPins[16];
};

class ShiftRegisterAddressBus : public AddressBus
{
public:
    ShiftRegisterAddressBus(const uint8_t latchPin, const uint8_t clockPin, const uint8_t dataPin);
    void setAddress(const unsigned short address) override;
    void setupPins(void);

private:
    const uint8_t _latchPin;
    const uint8_t _clockPin;
    const uint8_t _dataPin;
};

#endif