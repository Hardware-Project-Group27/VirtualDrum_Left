#ifndef PIEZO
#define PIEZO

#define SENSOR_COUNT 4

#define PIEZO1_PIN 34
#define PIEZO2_PIN 35
#define PIEZO3_PIN 36
#define PIEZO4_PIN 39

class Piezo{

    private:
        long LastTriggeredTime[SENSOR_COUNT];
        int LastReadValue[SENSOR_COUNT];
        int triggerLevel;
        void SendSerialTrigerSignal(int sensorId, int reading);
        Adafruit_SSD1306 display;
        WebSocketCon wsCon; 
    

    public:
        Piezo();
        int ReadPiezoInput(short c);
        void loop();
        void UpdateDisplay();
        static void Tougle();
        void PiezoInit(Adafruit_SSD1306 *d, WebSocketCon *ws);


};

#endif