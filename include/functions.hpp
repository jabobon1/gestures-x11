#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <chrono>
#include <initializer_list>
#include <X11/keysym.h>

/**
 * @brief Get the current date and time.
 *
 * @return A time_point object representing the current date and time.
 */
std::chrono::system_clock::time_point getDateNow();

/**
 * @brief Calculate the elapsed time in milliseconds since the given time point.
 *
 * @param lastDate A time_point object to measure the duration from.
 * @return The number of milliseconds as an integer that have elapsed since lastDate.
 */
int elapsed(std::chrono::system_clock::time_point lastDate);

/**
 * @brief Opens a file with restricted access rights.
 *
 * This function is typically used by libinput to open device files with specific permissions.
 *
 * @param path The path to the device file.
 * @param flags The flags for opening the file.
 * @param user_data Additional user data, if needed.
 * @return The file descriptor on success, or -1 on failure.
 */
static int open_restricted(const char *path, int flags, void *user_data);

/**
 * @brief Closes a file descriptor opened by open_restricted.
 *
 * This function is typically used by libinput to close device files opened by open_restricted.
 *
 * @param fd The file descriptor to close.
 * @param user_data Additional user data, if needed.
 */
static void close_restricted(int fd, void *user_data);

/**
 * @brief Initialize and get a libinput context for device handling.
 *
 * This function sets up libinput to interface with the input device system.
 *
 * @return A pointer to the initialized libinput context, or nullptr on failure.
 */
libinput *get_libinput_context();

/**
 * @brief Simulate a key press for a sequence of keys.
 *
 * This function simulates the pressing and releasing of a given sequence of keys.
 *
 * @param display The display connection to the X server.
 * @param keysyms A list of keysyms representing the keys to be pressed.
 */
void simulateKeyPress(Display *display, std::initializer_list<KeySym> keysyms);

#endif // FUNCTIONS_HPP
