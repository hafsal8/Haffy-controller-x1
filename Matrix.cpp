#include <Arduino.h>
#include "Matrix.h"

// ==============================
// HAFFY CONTROLLER X1
// 4x4 MATRIX PIN CONFIGURATION
// ==============================

// ROW1, ROW2, ROW3, ROW4
const byte rowPins[MATRIX_ROWS] = {4, 5, 6, 7};

// COL1, COL2, COL3, COL4
const byte colPins[MATRIX_COLS] = {8, 9, 10, 11};

// Debounce time
const unsigned long DEBOUNCE_DELAY = 20;

// Create Matrix object
MatrixClass Matrix;


// ==============================
// INITIALIZATION
// ==============================

void MatrixClass::begin()
{
    // Rows = OUTPUT
    for (byte row = 0; row < MATRIX_ROWS; row++)
    {
        pinMode(rowPins[row], OUTPUT);
        digitalWrite(rowPins[row], HIGH);
    }

    // Columns = INPUT_PULLUP
    for (byte col = 0; col < MATRIX_COLS; col++)
    {
        pinMode(colPins[col], INPUT_PULLUP);
    }

    // Initialize states
    for (byte row = 0; row < MATRIX_ROWS; row++)
    {
        for (byte col = 0; col < MATRIX_COLS; col++)
        {
            buttonState[row][col] = false;
            lastReading[row][col] = false;
            lastDebounceTime[row][col] = 0;
        }
    }
}


// ==============================
// MATRIX SCANNER
// ==============================

void MatrixClass::update()
{
    for (byte row = 0; row < MATRIX_ROWS; row++)
    {
        // Activate one row
        digitalWrite(rowPins[row], LOW);

        // Read all columns
        for (byte col = 0; col < MATRIX_COLS; col++)
        {
            // LOW = button pressed
            bool reading = (digitalRead(colPins[col]) == LOW);

            // Detect state change
            if (reading != lastReading[row][col])
            {
                lastDebounceTime[row][col] = millis();
                lastReading[row][col] = reading;
            }

            // Accept stable state after debounce time
            if (millis() - lastDebounceTime[row][col] >= DEBOUNCE_DELAY)
            {
                buttonState[row][col] = reading;
            }
        }

        // Deactivate this row
        digitalWrite(rowPins[row], HIGH);
    }
}


// ==============================
// READ BUTTON STATE
// ==============================

bool MatrixClass::isPressed(byte row, byte col)
{
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS)
    {
        return false;
    }

    return buttonState[row][col];
}