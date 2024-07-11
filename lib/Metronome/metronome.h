#ifndef METRONOME
#define METRONOME

#define PIN_MOTOR 14 // Motor pin
#define MIN_BPM 35   // Minimum BPM
#define MAX_BPM 250  // Maximum BPM

#define MIN_OUT 0
#define MAX_OUT 4096
#define MILLIS_IN_A_MINUTE 60000

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

class Metronome
{

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