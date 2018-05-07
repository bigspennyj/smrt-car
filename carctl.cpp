#include <string>
#include <thread>
#include <chrono>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <exception>
#include <limits.h>
#include <cerrno>

#include "carctl.h"

CarController::~CarController()
{
    if (fd > 0)
        close(fd);
}

CarController::CarController() : fd(-1)
{
    std::string filename = "/dev/i2c-1";

    if ((fd = open(filename.c_str(), O_RDWR)) < 0) {
        throw std::runtime_error("failed to open " + filename);
    }

    int addr = 0x18;
    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        close(fd);
        throw std::runtime_error("couldn't talk to slave");
    }
}

template<typename from_type, typename dest_type>
dest_type map_to_range(double value,
                      from_type fromLow,
                      from_type fromHigh,
                      dest_type toLow,
                      dest_type toHigh)
{
    return (toHigh-toLow) * (value-fromLow) / (fromHigh - fromLow) + toLow;
}

void CarController::steer(double angle)
{
    unsigned char buffer[6] = {0};
    uint16_t steer_val = map_to_range(angle, -1.0, 1.0, SERVO_MIN_PWM, SERVO_MAX_PWM);
    steer_val -= 100;
    if (steer_val <= SERVO_MIN_PWM)
        steer_val = SERVO_MIN_PWM;

    buffer[0] = STEER_SERVO;
    buffer[1] = steer_val >> 8;
    buffer[2] = steer_val & 0xff;
    if (write(fd, buffer, 3) != 3) {
        close(fd);
        perror("steer");
        throw std::runtime_error("couldn't write to servo1");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void CarController::accel(signed short accel)
{
    unsigned char buffer[6] = {0};
    uint16_t accel_val = map_to_range(accel,
                                      SHRT_MIN,
                                      SHRT_MAX,
                                      SERVO_MIN_PWM,
                                      SERVO_MAX_PWM);
    buffer[0] = ACCEL1;
    buffer[1] = accel_val >> 8;
    buffer[2] = accel_val & 0xff;
    if (write(fd, buffer, 3) != 3) {
        close(fd);
        throw std::runtime_error("couldn't write to accel");
    }

    buffer[0] = ACCEL2;
    if (write(fd, buffer, 3) != 3) {
        close(fd);
        perror("accel");
        throw std::runtime_error("couldn't write to accel");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
