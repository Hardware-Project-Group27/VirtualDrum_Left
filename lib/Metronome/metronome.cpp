#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "metronome.h"

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// int timer = 0;           // The higher the number, the slower the timing.
// int bpm = 0;

int motorState = LOW;             // Tracks the state of the LED
unsigned long previousMillis = 0; // Stores the last time LED was updated
unsigned long interval = 0;       // Interval between LED toggles based on BPM

int bpm = 120; // Initial BPM
int motorOnTime = 70;

bool lastIncButtonState = LOW;      // Variable to store the previous state of the increase button
bool lastDecButtonState = LOW;      // Variable to store the previous state of the decrease button
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
// unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

Metronome::Metronome()
{
}

void Metronome::MetronomeInit(Adafruit_SSD1306 *d)
{
  display = *d;
  pinMode(PIN_MOTOR, OUTPUT);
  analogWrite(PIN_MOTOR, 0);

  // Serial.begin(9600);
  // use a for loop to initialize each pin as an output:
  // display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
}

bool isRunning = false;
void Metronome::Tougle()
{
  isRunning = !isRunning;
  analogWrite(PIN_MOTOR, 0);
}

void Metronome::Open()
{
}

void Metronome::UpdateMetronome()
{
  if (isRunning)
  {
    interval = MILLIS_IN_A_MINUTE / bpm;

    unsigned long currentMillis = millis();
    unsigned long elapsedTime = currentMillis - previousMillis;

    if (motorState == HIGH && elapsedTime >= motorOnTime)
    {
      motorState = LOW;
      previousMillis = currentMillis;
      analogWrite(PIN_MOTOR, MIN_OUT);
    }
    else if (motorState == LOW && elapsedTime >= (interval - motorOnTime))
    {
      motorState = HIGH;
      previousMillis = currentMillis;
      analogWrite(PIN_MOTOR, MAX_OUT);
    }
  }
  else
  {
    analogWrite(PIN_MOTOR, 0);
  }
}

void Metronome::UpdateDisplay()
{
  // display.clearDisplay();
  // display.setTextSize(3);
  // display.setTextColor(WHITE);
  // display.setCursor(35, 5);
  // display.println("BPM:");
  // display.setCursor(15, 40);
  // display.println(bpm);
  // display.setTextSize(2);
  // display.setCursor(73, 45);
  // display.println("/min");
  // display.display();

  display.clearDisplay();
  display.setTextSize(1); // Small text size for status
  display.setTextColor(WHITE);

  // Display active or deactive status
  display.setCursor(10, 1);
  if (isRunning)
  {
    display.println("Status: active");
  }
  else
  {
    display.println("Status: deactive");
  }
  // Display BPM
  display.setTextSize(2); // Medium text size for BPM label
  display.setCursor(0, 25);
  display.println("BPM:");

  display.setTextSize(3); // Larger text size for BPM value
  display.setCursor(48, 20);
  display.println(bpm);

  display.setTextSize(1); // Small text size for units
  display.setCursor(100, 40);
  display.println("/min");

  display.display();
}

void Metronome::MetronomeUp()
{
  if (bpm < MAX_BPM)
  {
    bpm++;
    Serial.println(bpm);
  }
}

void Metronome::MetronomeDown()
{
  if (bpm > MIN_BPM)
  {
    bpm--;
    Serial.println(bpm);
  }
}
