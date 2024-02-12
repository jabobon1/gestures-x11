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

static int open_restricted(const char *path, int flags, void *user_data) {
    int fd = open(path, flags);
    return fd < 0 ? -1 : fd;
}

static void close_restricted(int fd, void *user_data) {
    close(fd);
}

static const struct libinput_interface interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};



libinput* get_libinput_context() {
    struct udev* udev = udev_new();
    if (!udev) {
        std::cerr << "Failed to initialize udev" << std::endl;
        return nullptr;
    }

    libinput* li = libinput_udev_create_context(&interface, nullptr, udev);
    if (!li) {
        std::cerr << "Failed to create libinput context" << std::endl;
        udev_unref(udev);
        return nullptr;
    }

    if (libinput_udev_assign_seat(li, "seat0") != 0) {
        std::cerr << "Failed to assign seat" << std::endl;
        libinput_unref(li);
        udev_unref(udev);
        return nullptr;
    }

    return li;
}



// Function to simulate key press using XTest (for X11)
void simulateKeyPress(Display* display, KeySym keysym) {
    KeyCode keycode = XKeysymToKeycode(display, keysym);
    if (keycode == 0) return; // Keycode not found

    XTestGrabControl(display, True); // Take control of the input
    XTestFakeKeyEvent(display, keycode, True, 0); // Press key
    XTestFakeKeyEvent(display, keycode, False, 0); // Release key
    XFlush(display); // Flush the X11 input queue
    XTestGrabControl(display, False); // Release control of the input
}

int main() {
    libinput* li = get_libinput_context();
    if (!li) return 1;

    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display" << std::endl;
        return 1;
    }

    // Set up polling
    struct pollfd fds;
    fds.fd = libinput_get_fd(li);
    fds.events = POLLIN;
    fds.revents = 0;


        // Main event loop
    while (true) {
        int n = poll(&fds, 1, -1); // Wait indefinitely for an event
        if (n > 0) {
            // We have an event, handle it
            libinput_dispatch(li);
            libinput_event* event;
            while ((event = libinput_get_event(li)) != nullptr) {
                if (libinput_event_get_type(event) == LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN) {
                    std::cout << "Gesture swipe begin detected" << std::endl;
                    simulateKeyPress(display, XK_Super_L); // Simulate Super/Windows key press
                } else if (libinput_event_get_type(event) == LIBINPUT_EVENT_GESTURE_SWIPE_END) {
                    std::cout << "Gesture swipe end detected" << std::endl;
                }
                libinput_event_destroy(event);
            };
        } else if (n == -1) {
            // Poll error
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        }
    }




    libinput_unref(li);
    XCloseDisplay(display);
    return 0;
}
