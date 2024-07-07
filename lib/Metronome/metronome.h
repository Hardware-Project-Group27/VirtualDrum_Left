#ifndef METRONOME
#define METRONOME

#define LED_PIN BUILTIN_LED // Pin for the LED
#define MIN_BPM 35       // Minimum BPM
#define MAX_BPM 250      // Maximum BPM

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET  -1
#define SCREEN_ADDRESS 0x3C

class Metronome{

private:
  Adafruit_SSD1306 display;


public:
Metronome();
void UpdateDisplay();
void MetronomeInit(Adafruit_SSD1306 *display);
void UpdateMetronome();
static void MetronomeUp();
static void MetronomeDown();
static void Tougle();
static void Open();

};

#endif