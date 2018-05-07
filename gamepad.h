#ifndef S_GAMEPAD_H
#define S_GAMEPAD_H

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <exception>
#include <string.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <utility>

class Gamepad
{
public: //methods

    Gamepad() : fd(-1), event(nullptr), current_state(), mtx(), go(true)
    {
        init();
    }

    ~Gamepad();

    struct GamepadState {
        GamepadState() : axes(), buttons() {}
        std::vector<signed short> axes;
        std::vector<signed short> buttons;
    };

    GamepadState get_state()
    { 
        std::lock_guard<std::mutex> g(mtx);

        GamepadState ret(current_state);
        return ret;
    }

    void run()
    {
        std::cout << "hello from the other thread" << std::endl;
        int nread;
        while (go)
        {
            if ((nread = read(fd, event, sizeof(js_event))) == sizeof(js_event)) {

                if (event->type & JS_EVENT_BUTTON) {
                    std::lock_guard<std::mutex> g(mtx);
                    current_state.buttons[event->number] = event->value;

                } else if (event->type & JS_EVENT_AXIS) {
                    std::lock_guard<std::mutex> g(mtx);
                    current_state.axes[event->number] = event->value;

                } else {
                    std::cout << "WARN: unhandled event" << std::endl;
                }
            } else if (nread < 0 && errno != EWOULDBLOCK) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        std::cout << "gamepad thread exiting..." << std::endl;
    }

private: //members
    int fd;
    js_event *event;
    GamepadState current_state;
    std::mutex mtx;
    std::thread t;
    std::atomic_bool go;

private: //methods

    void init();

};

#endif
