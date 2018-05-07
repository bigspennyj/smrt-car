#ifndef S_CARCTL_H
#define S_CARCTL_H

#include <iostream>

class CarController {
public:
    CarController();

    ~CarController();

    static constexpr uint8_t STEER_SERVO = 0;
    static constexpr uint16_t SERVO_MIN_PWM = 500;
    static constexpr uint16_t SERVO_MAX_PWM = 2500;
    static constexpr uint16_t SERVO_MIN_ANGLE = 0;
    static constexpr uint16_t SERVO_MAX_ANGLE = 180;
    static constexpr uint16_t MAX_ACCEL = 1000;
    static constexpr uint16_t ACCEL_STEP = 10;
    static constexpr uint16_t ACCEL1 = 4;
    static constexpr uint16_t ACCEL2 = 5;
    static constexpr int ADDRESS = 0x18;

    void steer(double angle);
    void accel(signed short accel);

private:
    int fd;
};

#endif
