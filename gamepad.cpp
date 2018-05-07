#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <iostream>
#include <errno.h>
#include <exception>
#include <string.h>
#include <thread>
#include <chrono>
#include "gamepad.h"

Gamepad::~Gamepad()
{
    go = false;
    t.join();
    if (fd > 0)
        close(fd);
}

void Gamepad::init()
{
    fd = open("/dev/input/js0", O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        throw std::runtime_error("error opening gamepad");
    }

    char name[256];
    __u8 axes = 0, buttons = 0;

    ioctl(fd, JSIOCGAXES, &axes);
    ioctl(fd, JSIOCGBUTTONS, &buttons);
    ioctl(fd, JSIOCGNAME(256), name);

    std::cout << "gamepad found: " << name << std::endl;
    std::cout << "# buttons: " << (int)buttons << std::endl;
    std::cout << "# axes: " << (int)axes << std::endl;

    current_state.axes.resize((int)axes);
    current_state.buttons.resize((int)buttons);

    event = new js_event();
    memset(event, 0, sizeof(js_event));

    std::cout << "Joystick initialized" << std::endl;

    std::cout << "starting poll thread" << std::endl;

    t = std::thread([this]() { run(); });
}
