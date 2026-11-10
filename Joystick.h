#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

class JoystickClass
{
public:
    int8_t LX = 0;
    int8_t LY = 0;
    int8_t RX = 0;
    int8_t RY = 0;

    bool L3 = false;
    bool R3 = false;

    void begin();
    void update();

private:
    int center[4];
    float filtered[4];

    int readMux(byte channel);
    int processAxis(byte channel);
};

extern JoystickClass Joystick;

#endif