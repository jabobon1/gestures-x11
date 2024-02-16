#ifndef GESTURES_MONITOR_HPP
#define GESTURES_MONITOR_HPP

#include <chrono>
#include <libinput.h>
#include <libudev.h>
#include <X11/extensions/XTest.h>
#include <poll.h>
#include "./functions.hpp"

const int REACTION_THRESHOLD_MS = 350; // Threshold for button press reaction in milliseconds

/**
 * Represents the state and timing of a swipe button press.
 */
struct SwipeButtonPressed
{
    bool lastPressedLeft = true; ///< Indicates if the last button pressed was the left button
    std::chrono::system_clock::time_point lastPressedMS = getDateNow(); ///< Timestamp of the last button press

    /**
     * Simulate a button press on the provided X display.
     *
     * @param display The X display where the button press should be simulated.
     * @param isLeft Specifies whether the left or right button should be pressed.
     */
    void pressButton(Display *display, bool isLeft);

    /**
     * Determine if enough time has passed since the last button press to consider it released.
     *
     * @param thresholdMs The threshold in milliseconds to determine button release.
     * @return True if the button is considered released, false otherwise.
     */
    bool isButtonReleased(int thresholdMs);
};

/**
 * Monitors touchpad gestures and triggers corresponding actions.
 */
class GesturesMonitor
{
private:
    libinput *li; ///< Pointer to the libinput context
    Display *display; ///< Pointer to the X11 display
    struct pollfd inputFds; ///< File descriptor set for input device polling
    SwipeButtonPressed buttonPressed; ///< State of the swipe button press

public:
    /**
     * Constructor for GesturesMonitor.
     * Initializes the libinput context and X11 display connection.
     */
    GesturesMonitor();

    /**
     * Destructor for GesturesMonitor.
     * Cleans up the libinput context and closes the X11 display connection.
     */
    ~GesturesMonitor();

    /**
     * Monitors and handles input events indefinitely until an exit condition is met.
     */
    void monitorEvents();

    /**
     * Handles a single input event from libinput.
     *
     * @param event The libinput event to handle.
     */
    void handleEvent(libinput_event *event);

    /**
     * Handles swipe gestures and performs corresponding actions.
     *
     * @param event The libinput event associated with a swipe gesture.
     */
    void handleSwipe(libinput_event *event);

    /**
     * Handles scroll gestures and performs corresponding actions.
     *
     * @param event The libinput event associated with a scroll gesture.
     */
    void handleScroll(libinput_event *event);
};

#endif // GESTURES_MONITOR_HPP
