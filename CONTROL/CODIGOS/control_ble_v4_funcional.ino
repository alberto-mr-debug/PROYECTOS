#include <Arduino.h>
#include <BleGamepad.h>

// BOTONES DIGITALES
#define X_BUTTON 15         // A
#define CIRCLE_BUTTON 17    // B
#define TRIANGLE_BUTTON 4   // Y
#define SQUARE_BUTTON 5     // X

#define R1_BUTTON 18
#define R2_BUTTON 19
#define L1_BUTTON 21
#define L2_BUTTON 22

#define START_BUTTON 23
#define SELECT_BUTTON 26
#define PS_BUTTON 25

// PULSADORES DE LOS JOYSTICKS
#define R3_BUTTON 33
#define L3_BUTTON 32

// JOYSTICKS ANALÓGICOS (ADC1)
#define RIGHT_VRX_JOYSTICK 34
#define RIGHT_VRY_JOYSTICK 35
#define LEFT_VRX_JOYSTICK  36 
#define LEFT_VRY_JOYSTICK  39 

#define NUM_BUTTONS 13

int buttonsPins[NUM_BUTTONS] = {
  X_BUTTON, CIRCLE_BUTTON, TRIANGLE_BUTTON, SQUARE_BUTTON,
  R1_BUTTON, R2_BUTTON, L1_BUTTON, L2_BUTTON,
  START_BUTTON, SELECT_BUTTON, PS_BUTTON,
  R3_BUTTON, L3_BUTTON
};

int androidGamepadButtons[NUM_BUTTONS] = {1, 2, 3, 4, 8, 10, 7, 9, 12, 11, 13, 15, 14};

BleGamepad bleGamepad("ESP32_BLE Gamepad", "ESP");
BleGamepadConfiguration bleGamepadConfig;

// Filtro de promedio para estabilizar lecturas
int readFilteredADC(int pin) {
  long sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += analogRead(pin);
  }
  return sum / 8;
}

// Mapeo a escala nativa HID de Android: 0 a 32767 (Centro = 16384)
int16_t processJoystickAndroid(int rawValue) {
  // Zona muerta en reposo (~1700 a 1950 entrega el centro exacto 16384)
  if (rawValue >= 1700 && rawValue <= 1950) {
    return 16384;
  }
  if (rawValue < 1700) {
    int val = map(rawValue, 0, 1700, 0, 16384);
    return constrain(val, 0, 16384);
  } else {
    int val = map(rawValue, 1950, 4095, 16384, 32767);
    return constrain(val, 16384, 32767);
  }
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonsPins[i], INPUT_PULLUP);
  }

  bleGamepadConfig.setAutoReport(false);
  bleGamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  bleGamepadConfig.setVid(0xe502);
  bleGamepadConfig.setPid(0xabcd);
  bleGamepadConfig.setHatSwitchCount(0);
  bleGamepad.begin(&bleGamepadConfig);
}

void loop() {
  if (bleGamepad.isConnected()) {
    // 1. Lectura procesada hacia escala Android (0 a 32767)
    int16_t lx = processJoystickAndroid(readFilteredADC(LEFT_VRX_JOYSTICK));
    int16_t ly = processJoystickAndroid(readFilteredADC(LEFT_VRY_JOYSTICK));
    int16_t rx = processJoystickAndroid(readFilteredADC(RIGHT_VRX_JOYSTICK));
    int16_t ry = processJoystickAndroid(readFilteredADC(RIGHT_VRY_JOYSTICK));

    // 2. Procesamiento de botones
    for (int i = 0; i < NUM_BUTTONS; i++) {
      bool pressed = !digitalRead(buttonsPins[i]);
      if (pressed) {
        bleGamepad.press(androidGamepadButtons[i]);
      } else {
        bleGamepad.release(androidGamepadButtons[i]);
      }
    }

    // 3. Asignación de ejes para Android
    bleGamepad.setLeftThumb(lx, ly);
    bleGamepad.setRightThumb(rx, ry);

    // 4. Envío de reporte BLE
    bleGamepad.sendReport();
    delay(15);
  }
}