#ifndef QUICK_BUTTON
#define QUICK_BUTTON

#include <Arduino.h>

enum QB_events
{
    NONE,
    SINGLE_CLICK,
    MULTIPLE_CLICK
};

typedef void (*durationClickCallback)(unsigned long);
typedef void (*countClickCallback)(int);
typedef void (*mixedClickCallback)(unsigned long, int);

class QuickButton
{
public:
    QuickButton(int pin, bool callbacks_auto_reset = true);
    ~QuickButton();
    void loop();
    void reset();
    bool isPressed();

    QB_events getEvent();
    int getClickCount();
    unsigned long getClickDuration();

    void virtualizeClick(QB_events event = SINGLE_CLICK,
                         unsigned long duration = 100,
                         int count = 1);

    void onClick(durationClickCallback callback);
    void onClick(countClickCallback callback);
    void onClick(mixedClickCallback callback);

private:
    const int _pin;
    const bool _callbacksAutoReset;

    durationClickCallback _durationClickCallback;
    countClickCallback _countClickCallback;
    mixedClickCallback _mixedClickCallback;

    QB_events _event;

    unsigned long _duration;
    unsigned long _deltaTime;
    unsigned long _pressEndTime;
    bool _pressed;
    int _count;
};

#endif