#include <libinput.h>
#include <libudev.h>
#include <X11/extensions/XTest.h>
#include <poll.h>
#include <./functions.hpp>

#ifndef GESTURES_MONITOR_HPP
#define GESTURES_MONITOR_HPP

const int REACTION_THRESHOLD_MS = 350; // 350 ms

struct SwipeButtonPressed
{
    bool lastPressedLeft = true;
    std::chrono::_V2::system_clock::time_point lastPressedMS = getDateNow();

    void pressButton(Display *display, bool isLeft);

    bool isButtonReleased(int thresholdMs);
};

class GesturesMonitor
{
private:
    libinput *li;
    Display *display;
    struct pollfd inputFds;
    SwipeButtonPressed buttonPressed;

public:
    GesturesMonitor();
    ~GesturesMonitor();

    void monitorEvents();
    void handleEvent(libinput_event *event);
    void handleSwipe(libinput_event *event);
    void handleScroll(libinput_event *event);
};

#endif // GESTURES_MONITOR_HPP