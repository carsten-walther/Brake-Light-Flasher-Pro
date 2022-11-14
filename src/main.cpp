#include <Arduino.h>
#include <EEPROM.h>

#include "config.h"

void flash(float frequency, uint8_t flashes) {
  
#ifdef DEBUG
  Serial.println("");
  Serial.println("Flash ...");
  Serial.print("Frequency:\t");
  Serial.println(frequency);
  Serial.print("Flashes:\t");
  Serial.println(flashes);
#endif

  float delayTime = ((1 / frequency) * 1000) / 2;

  int lightMin = map(LIGHT_MIN * 100, 0, 100, 0, 255);
  int lightMax = map(LIGHT_MAX * 100, 0, 100, 0, 255);
  
  for (int i = 0; i <= flashes; i++) {
    analogWrite(OUTPUT_PIN, lightMin);
    delay(delayTime);
    analogWrite(OUTPUT_PIN, lightMax);
    delay(delayTime);
  }

  delay(delayTime);
}

void sweep(uint8_t flashes) {

#ifdef DEBUG
  Serial.println("");
  Serial.println("Sweep ...");
  Serial.print("Flashes:\t");
  Serial.println(flashes);
#endif

  int lightMin = map(LIGHT_MIN * 100, 0, 100, 0, 255);
  int lightMax = map(LIGHT_MAX * 100, 0, 100, 0, 255);

  for (int i = 0; i <= flashes; i++) {
    for (int j = lightMin; j < lightMax; j++) {
      analogWrite(OUTPUT_PIN, j);
      delay(1);
    }

    for (int j = lightMax; j > lightMin; j--) {
      analogWrite(OUTPUT_PIN, j);
      delay(1);
    }
  }
}

void setup() {

#ifdef DEBUG
  Serial.begin(BAUDRATE);
  Serial.flush();
  Serial.println("");
#endif

  // initialize inputs and outputs
  pinMode(OUTPUT_PIN, OUTPUT);
  analogWrite(OUTPUT_PIN, 0);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  int buttonValue = digitalRead(BUTTON_PIN);

#ifdef DEBUG
  Serial.print("Button:\t\t");
  Serial.println(buttonValue);
#endif

  // read mode from eeprom
  int mode = (EEPROM.read(EEPROM_ADDR_MODE)) ? EEPROM.read(EEPROM_ADDR_MODE) : 0;

#ifdef DEBUG
  Serial.print("Mode:\t\t");
  Serial.println(mode);
#endif

  // if button is pressed, enter the setup mode
  if (buttonValue == HIGH) {

#ifdef DEBUG
  Serial.println("");
  Serial.println("Setting up ...");
#endif

    mode++;

    if (mode > MAX_MODE) {
      mode = 0;
    }

#ifdef DEBUG
  Serial.print("New Mode:\t\t");
  Serial.println(mode);
#endif

    EEPROM.write(EEPROM_ADDR_MODE, mode);

  }
    
  // flash brake light with mode
  switch (mode) {

    // mode 0 bypass: Frequency: 0 Hz, Duration: -, Flashes: 0
    default:
    case 0:
      // nothing to do, the brake light will have a solid state
      break;

    // mode 1: Frequency: 1.5 Hz, Duration: 2, Flashes: 3
    case 1:
      flash(1.5, 3);
      break;

    // mode 2: Frequency: 1.5 Hz, Duration: 5, Flashes: 8
    case 2:
      flash(1.5, 8);
      break;

    // mode 3: Frequency: 3 Hz, Duration: 2, Flashes: 6
    case 3:
      flash(1.5, 8);
      break;

    // mode 4: Frequency: 3 Hz, Duration: 5, Flashes: 15
    case 4:
      flash(3, 15);
      break;

    // mode 5: Frequency: 6 Hz, Duration: 2, Flashes: 12
    case 5:
      flash(6, 12);
      break;

    // mode 6: Frequency: 6 Hz, Duration: 5, Flashes: 30
    case 6:
      flash(6, 30);
      break;

    // mode 7: Frequency: 9 Hz, Duration: 2, Flashes: 18
    case 7:
      flash(9, 18);
      break;

    // mode 8: Frequency: 9 Hz, Duration: 5, Flashes: 45
    case 8:
      flash(9, 45);
      break;

    // mode 9: Frequency: 12 Hz, Duration: 2, Flashes: 24
    case 9:
      flash(12, 24);
      break;

    // mode 10: Frequency: 12 Hz, Duration: 5, Flashes: 60
    case 10:
      flash(12, 60);
      break;

    // mode 11 (deceleration warning signal): Frequency: sweeps from high to low frequency, Duration: 3, Flashes: 8
    case 11:
      sweep(8);
      break;
  }

  // activate solid brake light
  analogWrite(OUTPUT_PIN, 255);
}

void loop() {
  // Nothing to do here.
  // We do not need any looping functionalities.
}