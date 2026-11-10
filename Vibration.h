#ifndef VIBRATION_H
#define VIBRATION_H

#include <Arduino.h>

class VibrationClass
{
public:
    void begin();

    void left(uint8_t power, unsigned long duration);
    void right(uint8_t power, unsigned long duration);
    void both(uint8_t leftPower, uint8_t rightPower, unsigned long duration);

    void stop();
    void update();

private:
    unsigned long stopTime = 0;
    bool active = false;
};

extern VibrationClass Vibration;

#endif