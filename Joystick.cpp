#include <Arduino.h>
#include "Joystick.h"

// ==========================================
// HAFFY CONTROLLER X1
// JOYSTICK + 74HC4051
// ==========================================

#define S0 12
#define S1 13
#define S2 14

#define SIG 26

#define L3_PIN 2
#define R3_PIN 3

// Joystick settings
#define DEADZONE 20
#define FILTER_ALPHA 0.25f

JoystickClass Joystick;


// ==========================================
// 74HC4051 MUX
// ==========================================

int JoystickClass::readMux(byte channel)
{
    digitalWrite(S0, channel & 1);
    digitalWrite(S1, (channel >> 1) & 1);
    digitalWrite(S2, (channel >> 2) & 1);

    delayMicroseconds(5);

    return analogRead(SIG);
}


// ==========================================
// INITIALIZATION + CALIBRATION
// ==========================================

void JoystickClass::begin()
{
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);

    pinMode(L3_PIN, INPUT_PULLUP);
    pinMode(R3_PIN, INPUT_PULLUP);

    analogReadResolution(10);

    Serial.println("================================");
    Serial.println("HAFFY X1 JOYSTICK CALIBRATION");
    Serial.println("Keep both joysticks centered!");
    Serial.println("================================");

    long sum[4] = {0, 0, 0, 0};

    // 200 samples
    for (int i = 0; i < 200; i++)
    {
        for (int ch = 0; ch < 4; ch++)
        {
            sum[ch] += readMux(ch);
        }

        delay(5);
    }

    // Calculate center
    for (int ch = 0; ch < 4; ch++)
    {
        center[ch] = sum[ch] / 200;
        filtered[ch] = center[ch];
    }

    Serial.println();
    Serial.print("LX Center : ");
    Serial.println(center[0]);

    Serial.print("LY Center : ");
    Serial.println(center[1]);

    Serial.print("RX Center : ");
    Serial.println(center[2]);

    Serial.print("RY Center : ");
    Serial.println(center[3]);

    Serial.println();
    Serial.println("Calibration Complete");
    Serial.println("================================");
}


// ==========================================
// AXIS PROCESSING
// ==========================================

int JoystickClass::processAxis(byte channel)
{
    int raw = readMux(channel);

    // Low-pass filtering
    filtered[channel] =
        FILTER_ALPHA * raw +
        (1.0f - FILTER_ALPHA) * filtered[channel];

    int value = (int)filtered[channel];

    // Deadzone
    if (abs(value - center[channel]) < DEADZONE)
    {
        value = center[channel];
    }

    // Convert 0-1023 → -127 to +127
    value = map(value, 0, 1023, -127, 127);

    value = constrain(value, -127, 127);

    return value;
}


// ==========================================
// UPDATE
// ==========================================

void JoystickClass::update()
{
    LX = processAxis(0);
    LY = processAxis(1);

    RX = processAxis(2);
    RY = processAxis(3);

    // L3 / R3
    L3 = !digitalRead(L3_PIN);
    R3 = !digitalRead(R3_PIN);
}