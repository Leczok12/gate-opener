#include "QuickButton.h"
/*!
    @brief Constructor
    @param pin the pin to which the button is connected
    @param callbacks_auto_reset reset button state after invoke callback
    @note another button pin should be connected to ground
*/
QuickButton::QuickButton(int pin, bool callbacks_auto_reset)
    : _pin(pin),
      _callbacksAutoReset(callbacks_auto_reset),
      _durationClickCallback(nullptr),
      _countClickCallback(nullptr),
      _mixedClickCallback(nullptr),
      _event(QB_events::NONE),
      _duration(0),
      _deltaTime(0),
      _pressEndTime(0),
      _pressed(false),
      _count(0)
{
    pinMode(_pin, INPUT_PULLUP);
};

QuickButton::~QuickButton(){};

/*!
    @brief button main loop;
    @note call it at least once per one main loop
*/
void QuickButton::loop()
{
    if (_event != QB_events::NONE)
    {
        if (_durationClickCallback != nullptr ||
            _countClickCallback != nullptr ||
            _mixedClickCallback != nullptr)
        {
            if (_durationClickCallback != nullptr)
                _durationClickCallback(_duration);
            else if (_countClickCallback != nullptr)
                _countClickCallback(_count);
            else if (_mixedClickCallback != nullptr)
                _mixedClickCallback(_duration, _count);

            if (_callbacksAutoReset)
                reset();
        }
        return;
    }

    if (digitalRead(_pin) == LOW && !_pressed)
    {
        _deltaTime = millis();
        _pressed = true;
    }
    else if (digitalRead(_pin) == HIGH && _pressed)
    {
        _deltaTime = millis() - _deltaTime;
        _pressEndTime = millis();
        _pressed = false;

        if (_deltaTime > 50)
        {
            _duration = _deltaTime;
            _count++;
        }
    }

    if (_pressed)
        return;

    if (_count == 1 && _pressEndTime + 200 < millis())
    {
        _event = QB_events::SINGLE_CLICK;
        loop();
    }
    else if (_count > 1 && _pressEndTime + 200 < millis())
    {
        _event = QB_events::MULTIPLE_CLICK;
        loop();
    }
}

/*!
    @brief Reset button state (event, duration, count).
    @note call after read event, click count and duration.
*/
void QuickButton::reset()
{
    _event = QB_events::NONE;
    _duration = 0;
    _deltaTime = 0;
    _pressEndTime = 0;
    _pressed = false;
    _count = 0;
}

/*!
    @return True if is pressed.
*/
bool QuickButton::isPressed()
{
    return _pressed;
}
/*!
    @brief Get button event.
    @return current event
*/
QB_events QuickButton::getEvent()
{
    return _event;
}
/*!
    @brief Get button click count.
    @return click count.
*/
int QuickButton::getClickCount()
{
    return _count;
}
/*!
    @brief Get last click duration.
    @return click duration in mili seconds.
*/
unsigned long QuickButton::getClickDuration()
{
    return _duration;
}

/*!
    @brief Virtualize click and invoke callback if is's exist
    @param event EVENT type. NONE, SINGLE_CLICK, MULTIPLE_CLICK.
    @param duration duration of click. Greater than 100.
    @param count count of click. Greater than 0.
    @note If EVENT equals NONE, duartion is lower than 100,
          count is lower than 1 or button is pressed nothing will happend.
*/
void QuickButton::virtualizeClick(QB_events event, unsigned long duration, int count)
{
    if (!(event != QB_events::NONE && duration >= 100 && count >= 1) ||
        _pressed || _count != 0)
        return;

    _event = event;
    _duration = duration;
    _count = count;
    loop();
}

/*!
    @brief Set callback with only duration param.
    @param duration unsigned long type. Greater than 0.
*/
void QuickButton::onClick(durationClickCallback callback)
{
    _durationClickCallback = callback;
    _countClickCallback = nullptr;
    _mixedClickCallback = nullptr;
}
/*!
    @brief Set callback with only click count param.
    @param count int type. Greater than 0.
*/
void QuickButton::onClick(countClickCallback callback)
{
    _durationClickCallback = nullptr;
    _countClickCallback = callback;
    _mixedClickCallback = nullptr;
}
/*!
    @brief Set callback with duartion and click count param.
    @param duration unsigned long type. Greater than 0.
    @param count int type. Greater than 0.
*/
void QuickButton::onClick(mixedClickCallback callback)
{
    _durationClickCallback = nullptr;
    _countClickCallback = nullptr;
    _mixedClickCallback = callback;
}
