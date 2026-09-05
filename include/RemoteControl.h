#include <Arduino.h>

class RemoteControl
{
public:
    static void setup();
    static void update();

    static void handleCommand(uint8_t &command);
};