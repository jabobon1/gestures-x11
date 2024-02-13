#include <iostream>
#include <libinput.h>
#include <libudev.h>
#include <poll.h>
#include <cstring>
#include <chrono>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <../headers/functions.hpp>

const int REACTION_THRESHOLD_MS = 350; // 350 ms



struct SwipeButtonPressed
{
    bool lastPressedLeft = true;
    std::chrono::_V2::system_clock::time_point lastPressedMS = getDateNow();

    void pressButton(Display *display, bool isLeft)
    {
        lastPressedLeft = isLeft;
        lastPressedMS = getDateNow();
        if (isLeft)
        {
            simulateKeyPress(display, {XK_Alt_L, XK_Left}); // Go Back
        }
        else
        {
            simulateKeyPress(display, {XK_Alt_L, XK_Right});
        }
    };

    bool isButtonReleased()
    {
        return elapsed(lastPressedMS) > REACTION_THRESHOLD_MS;
    };
};

int main()
{
    libinput *li = get_libinput_context();
    if (!li)
        return 1;

    Display *display = XOpenDisplay(nullptr);
    if (!display)
    {
        std::cerr << "Failed to open X display" << std::endl;
        return 1;
    }

    // Set up polling
    struct pollfd fds;
    fds.fd = libinput_get_fd(li);
    fds.events = POLLIN;
    fds.revents = 0;

    SwipeButtonPressed buttonPressed = SwipeButtonPressed();

    // Main event loop
    while (true)
    {

        int n = poll(&fds, 1, -1); // Wait indefinitely for an event

        if (n > 0)
        {

            // We have an event, handle it
            libinput_dispatch(li);
            libinput_event *event;

            while ((event = libinput_get_event(li)) != nullptr)
            {
                if (libinput_event_get_type(event) == LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN)
                {
                    std::cout << "Gesture swipe begin detected" << std::endl;
                    simulateKeyPress(display, {XK_Super_L}); // Simulate Super/Windows key press
                }
                else if (libinput_event_get_type(event) == LIBINPUT_EVENT_GESTURE_SWIPE_END)
                {
                    std::cout << "Gesture swipe end detected" << std::endl;
                }
                else if (libinput_event_get_type(event) == LIBINPUT_EVENT_POINTER_SCROLL_FINGER)
                {
                    // std::cout << "LIBINPUT_EVENT_POINTER_SCROLL_FINGER detected" << std::endl;
                    libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(event);
                    
                    double dy = libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
                    double dx = libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);


                    if (dy == 0.0)
                    {
                        if (dx < -20) // Threshold for right swipe
                        {
                            if (buttonPressed.isButtonReleased() or buttonPressed.lastPressedLeft)
                            {
                                simulateKeyPress(display, {XK_Alt_L, XK_Right});
                            };
                        }
                        else if (dx > 20) // Threshold for left swipe

                        {
                            if (buttonPressed.isButtonReleased() or !buttonPressed.lastPressedLeft)
                            {
                                buttonPressed.pressButton(display, true);
                            };
                        }
                    }
                };

                libinput_event_destroy(event);
            };
        }
        else if (n == -1)
        {
            // Poll error
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }
    }

    libinput_unref(li);
    XCloseDisplay(display);
    return 0;
}
