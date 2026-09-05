class WebServer
{
public:
    static void setup();

    static void update();

private:
    static void setupMDNS();
    static void setupWebServer();
};