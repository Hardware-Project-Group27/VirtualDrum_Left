#include <SPI.h>
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "metronome.h"

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// int timer = 0;           // The higher the number, the slower the timing.
// int bpm = 0;

int ledState = LOW;               // Tracks the state of the LED
unsigned long previousMillis = 0; // Stores the last time LED was updated
float interval = 0;               // Interval between LED toggles based on BPM

int bpm = 120; // Initial BPM
int ledOnrate = 50;

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
  pinMode(LED_PIN, OUTPUT);
  // Serial.begin(9600);
  // use a for loop to initialize each pin as an output:
  // display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
}

bool isRunning = false;
void Metronome::Tougle()
{
  isRunning = !isRunning;
}

void Metronome::Open()
{
}

void Metronome::UpdateMetronome()
{
  if (isRunning)
  {
    interval = 60000000 / bpm;
    // Check if it's time to toggle the LED
    unsigned long currentMillis = micros();
    // if (currentMillis - previousMillis >= interval) {
    //   // Save the last time the LED was toggled
    //   previousMillis += interval;
    //   // Toggle the LED state
    //   ledState = !ledState;
    //   // Update the LED
    //   digitalWrite(LED_PIN, ledState);

    //   // Print the LED state
    //   if(ledState){
    //   Serial.println("led on");
    //   }
    //   else{
    //     Serial.println("led off");
    //   }
    // }

    if (ledState == HIGH && currentMillis - previousMillis >= ledOnrate)
    {
      // If the LED is on and the on duration has passed, turn it off
      ledState = LOW;
      previousMillis += (interval - ledOnrate); // Update the time when the state changed
      digitalWrite(LED_PIN, ledState);          // Turn off the LED
      Serial.println(bpm);
    }
    else if (ledState == LOW && currentMillis - previousMillis >= (interval - ledOnrate))
    {
      // If the LED is off and the off duration has passed, turn it on
      ledState = HIGH;
      previousMillis += ledOnrate;     // Update the time when the state changed
      digitalWrite(LED_PIN, ledState); // Turn on the LED
    }

    // Serial.println(bpm);
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
    display.setTextSize(1);  // Small text size for status
    display.setTextColor(WHITE);

    // Display active or deactive status
    display.setCursor(10, 1);
    if(isRunning){
      display.println("Status: active");
    }
    else{
      display.println("Status: deactive");
    }
    // Display BPM
    display.setTextSize(2);  // Medium text size for BPM label
    display.setCursor(0, 25);
    display.println("BPM:");
    
    display.setTextSize(3);  // Larger text size for BPM value
    display.setCursor(48, 20);
    display.println(bpm);

    display.setTextSize(1);  // Small text size for units
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
