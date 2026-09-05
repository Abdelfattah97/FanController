#include "Fan.h"
#include <Config.h>
#include <Buzzer.h>
#include <Debug.h>

// =========================
// Current fan state
// =========================

int fanState = STATE_OFF;

void Fan::setup()
{
    // Configure output pins
    pinMode(SPEED1_PIN, OUTPUT);
    pinMode(SPEED2_PIN, OUTPUT);
    pinMode(SPEED3_PIN, OUTPUT);

    // Configure initial state
    digitalWrite(SPEED1_PIN, LOW);
    digitalWrite(SPEED2_PIN, LOW);
    digitalWrite(SPEED3_PIN, LOW);
}

// Change Fan State
void Fan::switchState(int state)
{

    // Validate state
    if (state < STATE_OFF || state > STATE_SPEED3)
    {

        debugPrintln("Invalid fan state!");

        return;
    }

    Buzzer::beep(100, 1);

    if (state == fanState)
    {
        return;
    }

    // Get the pin associated with this state
    int pin = statePin[state];
    int prevPin = statePin[fanState];

    if (prevPin != 0)
    {

        digitalWrite(prevPin, LOW);

        delay(400);
    }

    // If state has an associated pin,
    // turn it ON
    if (pin != 0)
#include <Debug.h>
    {

        digitalWrite(pin, HIGH);
    }

    // Save the new state
    fanState = state;

    // =========================
    // Debug information
    // =========================

    debugPrint("Fan state: ");

    switch (fanState)
    {

    case STATE_OFF:
        debugPrintln("OFF");
        break;

    case STATE_SPEED1:
        debugPrintln("SPEED 1");
        break;

    case STATE_SPEED2:
        debugPrintln("SPEED 2");
        break;

    case STATE_SPEED3:
        debugPrintln("SPEED 3");
        break;
    }
}

int Fan::getState()
{
    return fanState;
}

bool Fan::isOn()
{
    return fanState != STATE_OFF;
}
