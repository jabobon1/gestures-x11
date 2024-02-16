#include <chrono>

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

std::chrono::_V2::system_clock::time_point getDateNow();

int elapsed(std::chrono::_V2::system_clock::time_point lastDate);

static int open_restricted(const char *path, int flags, void *user_data);

static void close_restricted(int fd, void *user_data);

libinput *get_libinput_context();

void simulateKeyPress(Display *display, std::initializer_list<KeySym> keysyms);

#endif // FUNCTIONS_HPP