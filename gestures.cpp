#include <iostream>
#include <libinput.h>
#include <libudev.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <chrono>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <initializer_list>
#include <vector>




const int REACTION_THRESHOLD_MS = 350; // 350 ms

std::chrono::_V2::system_clock::time_point getDateNow()
{
    return std::chrono::high_resolution_clock::now();
}

int elapsed(std::chrono::_V2::system_clock::time_point lastDate)
{
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    return duration_cast<milliseconds>(now - lastDate).count();
}

static int open_restricted(const char *path, int flags, void *user_data)
{
    int fd = open(path, flags);
    return fd < 0 ? -1 : fd;
}

static void close_restricted(int fd, void *user_data)
{
    close(fd);
}

static const struct libinput_interface interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

libinput *get_libinput_context()
{
    struct udev *udev = udev_new();
    if (!udev)
    {
        std::cerr << "Failed to initialize udev" << std::endl;
        return nullptr;
    }

    libinput *li = libinput_udev_create_context(&interface, nullptr, udev);
    if (!li)
    {
        std::cerr << "Failed to create libinput context" << std::endl;
        udev_unref(udev);
        return nullptr;
    }

    if (libinput_udev_assign_seat(li, "seat0") != 0)
    {
        std::cerr << "Failed to assign seat" << std::endl;
        libinput_unref(li);
        udev_unref(udev);
        return nullptr;
    }

    return li;
}

// Simulate key press for multiple keys
void simulateKeyPress(Display *display, std::initializer_list<KeySym> keysyms)
{
    // Take control of the input
    XTestGrabControl(display, True);

    // Press keys in the order provided
    for (KeySym keysym : keysyms)
    {
        KeyCode keycode = XKeysymToKeycode(display, keysym);
        if (keycode == 0)
            continue; // Skip if keycode not found

        XTestFakeKeyEvent(display, keycode, True, 0); // Press key
        XFlush(display);                              // Flush after each event for immediate effect
    }

    // Release keys in reverse order
    for (auto it = keysyms.end(); it != keysyms.begin();)
    {
        --it; // Decrement iterator to get the correct element
        KeyCode keycode = XKeysymToKeycode(display, *it);
        if (keycode == 0)
            continue; // Skip if keycode not found

        XTestFakeKeyEvent(display, keycode, False, 0); // Release key
        XFlush(display);                               // Flush after each event for immediate effect
    }

    // Release control of the input
    XTestGrabControl(display, False);
}

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
    bool lastPressedLeft = true;
    auto lastPressedMS = getDateNow();

      // Main event loop
    while (true)
    {

        int n = poll(&fds, 1, -1); // Wait indefinitely for an event

        if (n > 0)
        {

            // We have an event, handle it
            libinput_dispatch(li);
            libinput_event *event;
            // double swipe_dx = 0; // Track horizontal movement

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

                    // std::cout << "dy " << dy << " dx " << dx << std::endl;

                    if (dy == 0.0)
                    {
                        if (dx < -20)
                        {

                            std::cout << "PRESS XK_Right" << std::endl;
                            if (elapsed(lastPressedMS) > REACTION_THRESHOLD_MS or lastPressedLeft)
                            // Threshold for right swipe
                            {
                                simulateKeyPress(display, {XK_Alt_L, XK_Right});
                                lastPressedLeft = false;
                                lastPressedMS = getDateNow();
                            }; // Go Forward
                        }
                        else if (dx > 20)
                        {
                            std::cout << "PRESS XK_Left" << std::endl;
                            //             // Threshold for left swipe
                            if (elapsed(lastPressedMS) > REACTION_THRESHOLD_MS or !lastPressedLeft)
                            {
                                simulateKeyPress(display, {XK_Alt_L, XK_Left}); // Go Back
                                lastPressedLeft = true;
                                lastPressedMS = getDateNow();
                            };
                        }
                    }
                };
                // else if (libinput_event_get_type(event) == LIBINPUT_EVENT_GESTURE_SWIPE_END)

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
