#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "menu.h"


unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers




// const char* menuItems[] = {"Metronome", "Battery", "Reset", "Exit"};
const int menuLength = 4;

typedef struct MenuItem{
  char* menuItem;
  void (*callback)(void);
} MenuItem;

MenuItem mi[4];
int mlen =0;

int selectedMenuIndex = 0;
int textSize = 2;


Menu::Menu(){
  Serial.println("menu obj created.");
}

void Menu::MenuInit(Adafruit_SSD1306 *d) {
  display = *d;
  // start the serial conn
  // Serial.begin(9600);

  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }


  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(22,10);
  display.println("Group27");// Print text
  display.setCursor(30,40);
  display.println("Drum");
  display.display();
  delay(2000);

}



void Menu::MenuSetItem(char* displayName , void (*callback)(void) , int index){
  if(index==-1){
    index = mlen;
    mlen++;
  }
  mi[index].menuItem = displayName;
  mi[index].callback = callback;
}

void Menu::UpdateMenu(){
  display.clearDisplay();
  int lineHeight = 8 * textSize;
  for (int i = 0; i < menuLength; i++) {
    if (i == selectedMenuIndex) {
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Highlight selection
    } else {
      display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }
    display.setCursor(0, i * lineHeight);
    display.println(mi[i].menuItem);
  }
  display.display();
}

void Menu::MenuUp(){
      selectedMenuIndex--;
      if (selectedMenuIndex < 0) selectedMenuIndex = menuLength - 1;
      Serial.print("UP");
      Serial.println(selectedMenuIndex);
}


void Menu::MenuBack(){
  Serial.println("BACK");
}

void Menu::MenuDown(){
    selectedMenuIndex++;
    if (selectedMenuIndex >= menuLength) selectedMenuIndex = 0;
    Serial.print("DOWN");
    Serial.println(selectedMenuIndex);
}

int Menu::getSelectedIndex(){
  return selectedMenuIndex;
}

void Menu::DebugData(){
  Serial.print("selected index:");
  Serial.println(selectedMenuIndex);
  Serial.print("mlen:");
  Serial.println(mlen);

}

void Menu::MenuSelect(){
      DebugData();
      Serial.print("Selected: ");
      Serial.println(mi[selectedMenuIndex].menuItem);
      mi[selectedMenuIndex].callback();
}
