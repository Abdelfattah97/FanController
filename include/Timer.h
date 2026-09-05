class Timer
{
public:
    static void update();

    static void setFanOffTimer(unsigned int duration);
    static void cancelFanOffTimer();
    static bool isFanOffTimerActive();
    static unsigned long getRemainingFanOffMillis();
    static unsigned int getRemainingFanOffMins();
};