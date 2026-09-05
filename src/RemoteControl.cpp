#include <RemoteControl.h>
#include <IRremote.hpp>
#include <Config.h>
#include <FanService.h>
#include <Debug.h>

void RemoteControl::setup()
{
    // =========================
    // Start IR receiver
    // =========================

    IrReceiver.begin(
        IR_RECEIVE_PIN,
        ENABLE_LED_FEEDBACK);

    debugPrintln();
    debugPrintln("==============================");
    debugPrintln("       FAN REMOTE");
    debugPrintln("==============================");
    debugPrintln("Waiting for remote...");
}

void RemoteControl::update()
{
    // =====================================================
    // IR
    // =====================================================

    if (IrReceiver.decode())
    {

        // Ignore NEC repeat frames
        if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT))
        {

            // =========================
            // Debug hook: report every decoded frame, regardless of
            // whether it matches our known remote. This is what makes
            // it possible to "learn" a brand new remote - you see
            // raw decodes even before REMOTE_ADDRESS/commands match.
            // Compiles to nothing in production.
            // =========================

            uint16_t address = IrReceiver.decodedIRData.address;
            uint8_t command = IrReceiver.decodedIRData.command;
            bool accepted = IrReceiver.decodedIRData.protocol == NEC && address == REMOTE_ADDRESS;
            debugRemoteCommand(address, command, accepted);

            // =========================
            // Check remote
            // =========================

            if (accepted)
            {

                handleCommand(command);
            }
        }
        // =========================
        // Prepare IR receiver
        // =========================

        IrReceiver.resume();
    }
}

void RemoteControl::handleCommand(uint8_t &command)
{
    if (command == BUTTON_OFF)
    {
        debugPrintln("BUTTON OFF");

        fanService.setFanState(STATE_OFF);
    }
    else if (command == BUTTON_1)
    {
        debugPrintln("BUTTON 1");

        fanService.setFanState(STATE_SPEED1);
    }
    else if (command == BUTTON_2)
    {

        debugPrintln("BUTTON 2");

        fanService.setFanState(STATE_SPEED2);
    }
    else if (command == BUTTON_3)
    {
        debugPrintln("BUTTON 3");

        fanService.setFanState(STATE_SPEED3);
    }
    else if (command == BUTTON_WIFI_SWITCH)
    {

        debugPrintln("Switch Wifi");
        fanService.toggleWifi();
    }
    else if (command == BUTTON_MUTE_SWITCH)
    {
        debugPrintln("Switch Mute");
        fanService.toggleMute();
    }
    else
    {

        debugPrintln("Unknown command from our remote");
    }
}