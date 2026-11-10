#include <Arduino.h>
#include "Adafruit_TinyUSB.h"
#include "Joystick.h"
#include "Matrix.h"

// HAFFY X1 custom USB HID report
// 18 buttons + 4 signed 8-bit axes
// Total report size: 7 bytes

uint8_t const haffy_report_descriptor[] = {
  0x05, 0x01,       // Usage Page (Generic Desktop)
  0x09, 0x05,       // Usage (Game Pad)
  0xA1, 0x01,       // Collection (Application)

  // 18 buttons: 18 bits
  0x05, 0x09,       // Usage Page (Button)
  0x19, 0x01,       // Usage Minimum (Button 1)
  0x29, 0x12,       // Usage Maximum (Button 18)
  0x15, 0x00,       // Logical Minimum (0)
  0x25, 0x01,       // Logical Maximum (1)
  0x75, 0x01,       // Report Size (1 bit)
  0x95, 0x12,       // Report Count (18)
  0x81, 0x02,       // Input (Data, Variable, Absolute)

  // 6 padding bits: button section becomes 3 bytes
  0x75, 0x01,       // Report Size (1 bit)
  0x95, 0x06,       // Report Count (6)
  0x81, 0x03,       // Input (Constant)

  // Four signed axes: X, Y, Z, Rz
  0x05, 0x01,       // Usage Page (Generic Desktop)
  0x09, 0x30,       // Usage (X)
  0x09, 0x31,       // Usage (Y)
  0x09, 0x32,       // Usage (Z)
  0x09, 0x35,       // Usage (Rz)
  0x15, 0x81,       // Logical Minimum (-127)
  0x25, 0x7F,       // Logical Maximum (127)
  0x75, 0x08,       // Report Size (8 bits)
  0x95, 0x04,       // Report Count (4)
  0x81, 0x02,       // Input (Data, Variable, Absolute)

  0xC0              // End Collection
};

Adafruit_USBD_HID usb_hid;
uint8_t report[7];

void setup() {
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  Joystick.begin();
  Matrix.begin();

  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(
    haffy_report_descriptor,
    sizeof(haffy_report_descriptor)
  );
  usb_hid.begin();

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.println("HAFFY Controller X1 started");
}

void loop() {
#ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
#endif

  Joystick.update();
  Matrix.update();

  if (!usb_hid.ready()) {
    delay(2);
    return;
  }

  // Clear all 7 report bytes
  for (byte i = 0; i < sizeof(report); i++) {
    report[i] = 0;
  }

// Map physical matrix positions to logical USB button indices
const byte buttonMap[4][4] = {
  {0, 1, 2, 3},       // A, B, C, D
  {4, 7, 6, 5},       // UP, RIGHT, DOWN, LEFT
  {8, 9, 10, 11},     // L1, L2, R1, R2
  {13, 14, 12, 15}    // START, HOME, SELECT, EXTRA
};
  for (byte row = 0; row < MATRIX_ROWS; row++) {
    for (byte col = 0; col < MATRIX_COLS; col++) {
      if (Matrix.isPressed(row, col)) {
        byte index = buttonMap[row][col];
        report[index / 8] |= (1U << (index % 8));
      }
    }
  }

  // L3 = button 17, R3 = button 18
  if (Joystick.L3) report[2] |= (1U << 0);
  if (Joystick.R3) report[2] |= (1U << 1);

  // Axes occupy bytes 3, 4, 5, 6
  report[3] = (uint8_t)(int8_t)Joystick.LX;
  report[4] = (uint8_t)(int8_t)Joystick.LY;
  report[5] = (uint8_t)(int8_t)Joystick.RX;
  report[6] = (uint8_t)(int8_t)Joystick.RY;

  usb_hid.sendReport(0, report, sizeof(report));

  delay(2);
}