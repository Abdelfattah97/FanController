#include "Timer.h"
#include <Fan.h>
#include <Config.h>

namespace
{
    unsigned long fanOffTimerEndTime = 0;
    bool fanOffTimerActive = false;
}

// =====================================================
// Update
// =====================================================

void Timer::update()
{
    if (!fanOffTimerActive)
        return;

    // Timer expired
    if (getRemainingFanOffMillis() == 0)
    {
        Fan::switchState(STATE_OFF);
        fanOffTimerActive = false;
        return;
    }

    // Fan was turned off externally
    if (!Fan::isOn())
    {
        cancelFanOffTimer();
    }
}

// =====================================================
// Set fan off timer
// =====================================================

void Timer::setFanOffTimer(unsigned int afterMins)
{
    unsigned long durationMillis =
        afterMins * 60UL * 1000UL;

    fanOffTimerEndTime = millis() + durationMillis;
    fanOffTimerActive = true;

    // Start fan if it is currently off
    if (!Fan::isOn())
    {
        Fan::switchState(STATE_SPEED1);
    }

    Serial.print("Fan off timer set for ");
    Serial.print(afterMins);
    Serial.println(" minutes.");
}

// =====================================================
// Cancel fan off timer
// =====================================================

void Timer::cancelFanOffTimer()
{
    fanOffTimerActive = false;

    Serial.println("Fan off timer canceled.");
}

// =====================================================
// Timer state
// =====================================================

bool Timer::isFanOffTimerActive()
{
    return fanOffTimerActive;
}

// =====================================================
// Remaining time
// =====================================================

unsigned long Timer::getRemainingFanOffMillis()
{
    if (!fanOffTimerActive)
        return 0;

    unsigned long now = millis();

    // Timer expired.
    // Signed subtraction makes this safe across millis() rollover.
    if ((long)(now - fanOffTimerEndTime) >= 0)
        return 0;

    return fanOffTimerEndTime - now;
}

// =====================================================
// Remaining minutes
// =====================================================

unsigned int Timer::getRemainingFanOffMins()
{
    return getRemainingFanOffMillis() / 60000UL;
}