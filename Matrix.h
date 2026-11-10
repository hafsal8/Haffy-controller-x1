#ifndef MATRIX_H
#define MATRIX_H

#include <Arduino.h>

#define MATRIX_ROWS 4
#define MATRIX_COLS 4

class MatrixClass
{
public:
    void begin();
    void update();

    bool isPressed(byte row, byte col);

private:
    bool buttonState[MATRIX_ROWS][MATRIX_COLS];
    bool lastReading[MATRIX_ROWS][MATRIX_COLS];
    unsigned long lastDebounceTime[MATRIX_ROWS][MATRIX_COLS];
};

extern MatrixClass Matrix;

#endif