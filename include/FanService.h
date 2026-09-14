#pragma once

#include <Arduino.h>
#include <StateBroadcaster.h>

class FanService
{
public:
    explicit FanService(StateBroadcaster &stateBroadcaster);

    void update();

    void setFanState(int state);
    void mute(bool flag);
    void toggleMute();
    void setWifiEnabled(bool enabled);
    void toggleWifi();
    void setTimer(unsigned int minutes);
    void cancelTimer();

    String getStatusJson() const;
    String getTimerStatusJson() const;

private:
    StateBroadcaster &stateBroadcaster;

    const char *getFanStateString() const;
    void setDefaultTimer(unsigned int minutes);
    void broadcastStatus();
};

extern FanService fanService;
