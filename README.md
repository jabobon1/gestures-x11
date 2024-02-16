# Gestures-X11

Gestures-X11 is a solution designed to enhance the interactivity of the X11 display server environment. It allows users to intuitively control their desktop experience with touchpad gestures, bringing a touch of convenience to Linux systems.

## Features

- **Swipe Gestures**: Navigate through your browser history or switch between workspaces with simple swipe gestures.
- **Gesture Recognition**: Gestures-X11 recognizes swipe beginnings and finger scrolling, triggering corresponding actions seamlessly.

## Installation

To install Gestures-X11, clone the repository and compile the project:

```bash
git clone https://github.com/jabobon1/gestures-x11.git
cd gestures-x11
# builds the program in build/ directory
make
# moves binary file to /usr/local/bin/ and creates gestures-x11.service # file for systemd
make install
```
### After instalation don't forget to start the service: 
```bash
sudo systemctl enable gestures-x11.service # to enable the service
sudo systemctl start gestures-x11.service # to start the service
```