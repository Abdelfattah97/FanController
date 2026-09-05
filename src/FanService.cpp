#include <FanService.h>
#include <Buzzer.h>
#include <Config.h>
#include <Fan.h>
#include <Timer.h>
#include <WifiManager.h>

FanService::FanService(StateBroadcaster &stateBroadcaster)
    : stateBroadcaster(stateBroadcaster)
{
}

void FanService::update()
{
    String previousStatus = getStatusJson();
    Timer::update();

    if (getStatusJson() != previousStatus)
    {
        broadcastStatus();
    }
}

void FanService::setFanState(int state)
{
    Fan::switchState(state);
    broadcastStatus();
}

void FanService::mute(bool flag)
{
    Buzzer::mute(flag);
    broadcastStatus();
}

void FanService::toggleMute()
{
    mute(!isMuted);
}

void FanService::setWifiEnabled(bool enabled)
{
    WifiManager::switchWifi(!enabled);
}

void FanService::toggleWifi()
{
    setWifiEnabled(stopWifi);
}

void FanService::setTimer(unsigned int minutes)
{
    Timer::setFanOffTimer(minutes);
    broadcastStatus();
}

void FanService::cancelTimer()
{
    Timer::cancelFanOffTimer();
    broadcastStatus();
}

String FanService::getStatusJson() const
{
    String json = "{";

    json += "\"state\":\"";
    json += getFanStateString();
    json += "\"";

    json += ",\"mute\":";
    json += isMuted ? "true" : "false";

    json += ",\"timer\":";
    json += Timer::isFanOffTimerActive()
                ? String(Timer::getRemainingFanOffMins())
                : "null";

    json += "}";

    return json;
}

String FanService::getTimerStatusJson() const
{
    String json = "{\"timer\":";

    json += Timer::isFanOffTimerActive()
                ? String(Timer::getRemainingFanOffMins())
                : "null";

    json += "}";
    return json;
}

const char *FanService::getFanStateString() const
{
    switch (Fan::getState())
    {
    case STATE_SPEED1:
        return "SPEED1";
    case STATE_SPEED2:
        return "SPEED2";
    case STATE_SPEED3:
        return "SPEED3";
    case STATE_OFF:
    default:
        return "OFF";
    }
}

void FanService::broadcastStatus()
{
    stateBroadcaster.broadcast(getStatusJson());
}

FanService fanService(stateBroadcaster);
