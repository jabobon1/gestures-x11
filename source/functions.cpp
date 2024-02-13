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
#include <../headers/functions.hpp>

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