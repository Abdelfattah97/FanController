class WifiManager
{
public:
    static void setup();
    static void update();
    static void switchWifi(bool flag);

private:
    static bool connectWiFi();
    static void handleWiFi();
};