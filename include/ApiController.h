class ApiController
{
public:
    static void handleRoot();
    static void handleNotFound();
    static void handleCSS();
    static void handleJavaScript();
    static void handleFanStatus();
    static void handleFanOff();
    static void handleFanSpeed1();
    static void handleFanSpeed2();
    static void handleFanSpeed3();
    static void handleBeepMute();
    static void handleBeepUnmute();
    static void handleWificonnect();
    static void handleWifiDisconnect();
    static void handleSetFanOffTimer(int mins);
    static void handleCancelFanOffTimer();
    static void handleDebugPage();
};
