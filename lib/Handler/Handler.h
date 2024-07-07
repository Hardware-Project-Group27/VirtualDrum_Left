#ifndef HANDLER
#define HANDLER

#define WINDOW_HOME 0
#define WINDOW_MENU 1
#define WINDOW_METRONOME 2
#define WINDOW_BATTERY 3
#define WINDOW_PIEZO 4

#define SHORT_PRESS_UPPER_TIME 800
#define MEDIUM_PRESS_UPPER_TIME 3000

class Handler{

    private:
        typedef void (*HandleOp)(void);
        HandleOp upFunc;
        HandleOp downFunc;
        HandleOp selectFunc;
        HandleOp backFunc;

    public:
        Handler();
        void setFucnctions(HandleOp upFunc, HandleOp downFunc, HandleOp selectFunc, HandleOp backFunc);
        void Up();
        void set_UpFunc(HandleOp upFunc);
        void Down();
        void set_DownFunc(HandleOp downFunc);
        void Select();
        void set_SelectFunc(HandleOp selectFunc);
        void Back();
        void set_BackFunc(HandleOp backFunc);
        static short currentWindow;
};

class Btn{

    private:
        void shortPressAction();
        void mediumPressAction();
        void longPressAction();
        short pin;
        void (*trigerFunction)(void);
        bool buttonState = false;          // Current state of the button
        bool lastButtonState = false;      // Previous state of the button
        unsigned long buttonPressTime = 0; // Time when the button was pressed
        unsigned long buttonReleaseTime = 0; // Time when the button was released
        unsigned long buttonDuration = 0;  // Duration for which the button was pressed
        const unsigned long debounceDelay = 50; // Debounce time in milliseconds
        bool isDone=false;
        unsigned long mFuncLastCall=0;
        unsigned long LFuncLastCall=0;

    public:
        Btn(short pin);
        void setup(void (*trigerFunction)(void));
        void check();

};

#endif