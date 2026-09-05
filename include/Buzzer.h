class Buzzer
{
public:
    static void setup();
    static void beep(int interval);
    static void beep(int interval, int times);
    static void mute(bool flag);
};