#include <Arduino.h>
#include <EEPROM.h>

#include "Config.h"

int mode = 0;                     // rendering mode
bool rendering = false;           // rendering is in progress
unsigned long previousMillis = 0; // timer for button
byte longKeyPressCount = 0;       // counter for button press time
byte prevKeyState = LOW;          // last button state

void flash(float frequency, uint8_t flashes)
{

  float delayTime = ((1 / frequency) * 1000) / 2;

#ifdef DEBUG
  Serial.println("");
  Serial.println("Flashing");
  Serial.print("Frequency:\t");
  Serial.print(frequency);
  Serial.println(" Hz");
  Serial.print("Flashes:\t");
  Serial.println(flashes);
  Serial.print("Duration:\t");
  Serial.print((flashes * (delayTime * 2) / 1000));
  Serial.println(" sec");
#endif

  int lightMin = map(LIGHT_MIN * 100, 0, 100, 0, 255);
  int lightMax = map(LIGHT_MAX * 100, 0, 100, 0, 255);

  for (int i = 0; i < flashes; i++)
  {
    if (i == 0)
    {
      analogWrite(OUTPUT_PIN, 255);
      delay(delayTime);
      analogWrite(OUTPUT_PIN, lightMin);
      delay(delayTime);
    }
    else
    {
      analogWrite(OUTPUT_PIN, lightMax);
      delay(delayTime);
      analogWrite(OUTPUT_PIN, lightMin);
      delay(delayTime);
    }
  }
}

void sweep(uint8_t cycles)
{
#ifdef DEBUG
  Serial.println("");
  Serial.println("Sweeping");
  Serial.print("Cycles:\t");
  Serial.println(cycles);
#endif

  int lightMin = map(LIGHT_MIN * 100, 0, 100, 0, 255);
  int lightMax = map(LIGHT_MAX * 100, 0, 100, 0, 255);

  for (int i = 0; i <= cycles; i++)
  {
    for (int j = lightMin; j < lightMax; j++)
    {
      analogWrite(OUTPUT_PIN, j);
      delay(1);
    }

    for (int j = lightMax; j > lightMin; j--)
    {
      analogWrite(OUTPUT_PIN, j);
      delay(1);
    }
  }
}

void render()
{
#ifdef DEBUG
  Serial.println("");
  Serial.print("Rendering Mode:\t");
  Serial.println(mode);
#endif

  // flash brake light with mode
  switch (mode)
  {

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

void handleModeChange()
{
#ifdef DEBUG
  Serial.println("");
  Serial.println("Entering setup mode...");
#endif

  mode++;

  if (mode > MAX_MODE)
  {
    mode = 0;
  }

#ifdef DEBUG
  Serial.println("");
  Serial.print("New Mode:\t");
  Serial.println(mode);
#endif

  render();

  EEPROM.write(EEPROM_ADDR_MODE, mode);

  previousMillis = 0;
  longKeyPressCount = 0;
  prevKeyState = LOW;
}

void setup()
{
#ifdef DEBUG
  Serial.begin(BAUDRATE);
  Serial.flush();
  Serial.println("");
  Serial.println("Brake Light Flasher Pro");
  Serial.println("==========================");
  Serial.print("Version:\t");
  Serial.println(VERSION);
  Serial.println("");
#endif

  // initialize inputs and outputs
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(OUTPUT_PIN, OUTPUT);
  analogWrite(OUTPUT_PIN, 0);

  // read mode from eeprom
  mode = (EEPROM.read(EEPROM_ADDR_MODE)) ? EEPROM.read(EEPROM_ADDR_MODE) : 0;

  // render the brake light mode
  rendering = true;
  render();
  rendering = false;
}

void loop()
{
  if (millis() - previousMillis >= 1000)
  {
    previousMillis = millis();

    byte currKeyState = digitalRead(BUTTON_PIN);

    if ((prevKeyState == LOW) && (currKeyState == HIGH))
    {
      longKeyPressCount = 0;
    }
    else if ((prevKeyState == HIGH) && (currKeyState == LOW))
    {
      if (!rendering && longKeyPressCount >= 5)
      {
        handleModeChange();
      }
    }
    else if (currKeyState == HIGH)
    {
      longKeyPressCount++;
    }

    if (!rendering && longKeyPressCount >= 5)
    {
      // switch off brake light for setup mode
      analogWrite(OUTPUT_PIN, 0);
    }

    prevKeyState = currKeyState;
  }
}