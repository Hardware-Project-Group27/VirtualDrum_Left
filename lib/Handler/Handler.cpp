#include "Handler.h"
#include <Arduino.h>

short Handler::currentWindow = WINDOW_HOME;

Handler::Handler(){
    // Constructor
}

void Handler::setFucnctions(HandleOp upFunc, HandleOp downFunc, HandleOp selectFunc, HandleOp backFunc){
    // Static function to set all functions
    set_UpFunc(upFunc);
    set_DownFunc(downFunc);
    set_SelectFunc(selectFunc);
    set_BackFunc(backFunc);
}

void Handler::Up(){
    upFunc();
}

void Handler::set_UpFunc(HandleOp uf){
    upFunc = *uf;
}

void Handler::Down(){
    downFunc();
}

void Handler::set_DownFunc(HandleOp df){
    downFunc = *df;
}

void Handler::Select(){
    selectFunc();
}

void Handler::set_SelectFunc(HandleOp sf){
    selectFunc = *sf;
}

void Handler::Back(){
    backFunc();
}

void Handler::set_BackFunc(HandleOp bf){
    backFunc = *bf;
}


// Btn class
Btn::Btn(short p){
    pin = p;
    pinMode(pin, INPUT_PULLUP);
}

void Btn::setup(void (*tf)(void)){
    trigerFunction = *tf;
}

void Btn::check() {
  // Read the current state of the button:
  buttonState = digitalRead(pin) == LOW; // Active LOW

  // Check if the button state has changed:
  if (buttonState != lastButtonState) {
    // Reset the debounce timer
    if (buttonState) {
      buttonPressTime = millis(); // Button pressed, record press time
    } else {
      isDone =false;
      buttonReleaseTime = millis(); // Button released, record release time
      buttonDuration = buttonReleaseTime - buttonPressTime; // Calculate press duration

      // Print the button press duration after releasing the button
      Serial.print("Button press duration: ");
      Serial.print(buttonDuration);
      Serial.println(" ms");
    }
  }  
  // Serial.println( millis() -buttonPressTime);
  // Check if the button is pressed and print the time since it was pressed
  if (buttonState) {
    // Serial.println(millis() - buttonPressTime);
    if(millis() - buttonPressTime<SHORT_PRESS_UPPER_TIME){
      shortPressAction();
    }
    else if(millis() - buttonPressTime<MEDIUM_PRESS_UPPER_TIME){
      mediumPressAction();
    }
    else if(millis() - buttonPressTime<LONG_PRESS_UPPER_TIME){
      longPressAction();
    }
    else{
      longlongPressAction();
    }
  }

  // Update the last button state:
  lastButtonState = buttonState;
}

void Btn::shortPressAction() {
  if(!isDone){
    // Serial.println("Short press action");
    trigerFunction();
    isDone = !isDone;
  }
}

void Btn::mediumPressAction() {
  if(millis()-mFuncLastCall>500){
    // Serial.println("Medium press action");
    trigerFunction();
    mFuncLastCall = millis();
  }
}

void Btn::longPressAction() {
  if(millis()-LFuncLastCall>50){
    // Serial.println("Long press action");
    trigerFunction();
    LFuncLastCall = millis();
  }
}

void Btn::longlongPressAction() {
  Serial.println("Long long press action------------------------");
  trigerFunction();
}