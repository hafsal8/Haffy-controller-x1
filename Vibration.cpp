#include <Arduino.h>
#include "Vibration.h"

#define LEFT_MOTOR_PIN  20
#define RIGHT_MOTOR_PIN 21

VibrationClass Vibration;

void VibrationClass::begin()
{
    pinMode(LEFT_MOTOR_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_PIN, OUTPUT);

    stop();
}

void VibrationClass::left(uint8_t power, unsigned long duration)
{
    analogWrite(LEFT_MOTOR_PIN, power);
    analogWrite(RIGHT_MOTOR_PIN, 0);

    stopTime = millis() + duration;
    active = true;
}

void VibrationClass::right(uint8_t power, unsigned long duration)
{
    analogWrite(LEFT_MOTOR_PIN, 0);
    analogWrite(RIGHT_MOTOR_PIN, power);

    stopTime = millis() + duration;
    active = true;
}

void VibrationClass::both(uint8_t leftPower,
                          uint8_t rightPower,
                          unsigned long duration)
{
    analogWrite(LEFT_MOTOR_PIN, leftPower);
    analogWrite(RIGHT_MOTOR_PIN, rightPower);

    stopTime = millis() + duration;
    active = true;
}

void VibrationClass::stop()
{
    analogWrite(LEFT_MOTOR_PIN, 0);
    analogWrite(RIGHT_MOTOR_PIN, 0);

    active = false;
    stopTime = 0;
}

void VibrationClass::update()
{
    if (active && millis() >= stopTime)
    {
        stop();
    }
}