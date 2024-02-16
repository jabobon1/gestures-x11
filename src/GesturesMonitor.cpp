#include <iostream>
#include <cstring>
#include <libinput.h>
#include <libudev.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <poll.h>
#include <../include/functions.hpp>
#include <../include/GesturesMonitor.hpp>

void SwipeButtonPressed::pressButton(Display *display, bool isLeft)
{
    this->lastPressedLeft = isLeft;
    this->lastPressedMS = getDateNow();

    if (isLeft)
        simulateKeyPress(display, {XK_Alt_L, XK_Left}); // Go Back
    else
        simulateKeyPress(display, {XK_Alt_L, XK_Right}); // Go Further
};

bool SwipeButtonPressed::isButtonReleased(int thresholdMs)
{
    return elapsed(lastPressedMS) > thresholdMs;
};

GesturesMonitor::GesturesMonitor()
{

    this->li = get_libinput_context();
    if (!this->li)
        throw std::runtime_error("Failed to get libinput context");

    this->display = XOpenDisplay(nullptr);
    if (!this->display)
        throw std::runtime_error("Failed to open X display");

    this->inputFds = {libinput_get_fd(li), POLLIN, 0};
    this->buttonPressed = SwipeButtonPressed();
}

GesturesMonitor::~GesturesMonitor()
{
    libinput_unref(li);
    XCloseDisplay(display);
}

void GesturesMonitor::handleSwipe(libinput_event *event)
{
    std::cout << "Gesture swipe begin detected" << std::endl;
    simulateKeyPress(display, {XK_Super_L}); // Simulate Super/Windows key press
}

void GesturesMonitor::handleScroll(libinput_event *event)
{
    libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(event);

    double dy = libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
    double dx = libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);

    if (dy == 0.0)
    {
        if (dx < -20) // Threshold for right swipe
        {
            if (buttonPressed.isButtonReleased(REACTION_THRESHOLD_MS) or buttonPressed.lastPressedLeft)
                simulateKeyPress(display, {XK_Alt_L, XK_Right});
        }
        else if (dx > 20) // Threshold for left swipe

        {
            if (buttonPressed.isButtonReleased(REACTION_THRESHOLD_MS) or !buttonPressed.lastPressedLeft)
                buttonPressed.pressButton(display, true);
        }
    }
};

void GesturesMonitor::handleEvent(libinput_event *event)
{
    while ((event = libinput_get_event(li)) != nullptr)
    {
        if (libinput_event_get_type(event) == LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN)
            this->handleSwipe(event);

        else if (libinput_event_get_type(event) == LIBINPUT_EVENT_POINTER_SCROLL_FINGER)
            this->handleScroll(event);

        libinput_event_destroy(event);
    };
}

void GesturesMonitor::monitorEvents()
{
    // Main event loop
    while (true)
    {

        int n = poll(&this->inputFds, 1, -1); // Wait indefinitely for an event

        if (n > 0)
        {
            // We have an event, handle it
            libinput_dispatch(li);
            libinput_event *event;
            this->handleEvent(event);
        }
        else if (n == -1)
        {
            // Poll error
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }
    }
}

int main()
{
    GesturesMonitor gestureMonitor = GesturesMonitor();
    gestureMonitor.monitorEvents();
    return 0;
};