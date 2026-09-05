#include "Buzzer.h"
#include <Config.h>

bool isMuted = false;

void Buzzer::setup()
{

    // Configure output pins
    pinMode(BEEP_PIN, OUTPUT);
    // Configure initial state
    digitalWrite(BEEP_PIN, LOW);
}

void Buzzer::beep(int interval)
{
    beep(interval, 1);
}

void Buzzer::beep(int interval, int times)
{

    if (isMuted)
    {
        return;
    }

    for (int x = 0; x < times; x++)
    {

        digitalWrite(BEEP_PIN, HIGH);

        delay(interval);

        digitalWrite(BEEP_PIN, LOW);

        delay(interval);
    }
}

void Buzzer::mute(bool flag)
{
    isMuted = flag;
}
