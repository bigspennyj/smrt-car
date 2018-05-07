#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <limits.h>
#include <cmath>
#include <sstream>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "gamepad.h"
#include "carctl.h"

template<typename from_type, typename dest_type>
dest_type map_to_range(int value,
                      from_type fromLow,
                      from_type fromHigh,
                      dest_type toLow,
                      dest_type toHigh)
{
    return (toHigh-toLow)*(value-fromLow) / (fromHigh-fromLow) + toLow;
}

int main()
{
    Gamepad gpad;
    CarController carctl;
    long ms_sleep = (1.0 / 60.0) * 1000;

    try
    {
        double steerVal = 0;

        double limitFPS = 2.0/15.0;
        auto lastTime = std::chrono::steady_clock::now();
        double deltaTime = 0;

	auto capture = [&gpad]() {
		cv::VideoCapture vidCap(0);
		vidCap.set(CV_CAP_PROP_FPS, 8);
		std::ofstream gpad_file("./output/steer.log");

		cv::Mat frame;

		int frameNo = 0;
		while (frameNo <= 800) {

			vidCap >> frame;
			auto state = gpad.get_state();

			std::stringstream filename;
			filename << "./output/" << frameNo << ".jpg";
			cv::imwrite(filename.str(), frame);
            		
			double thisVal = map_to_range(state.axes[0], SHRT_MIN, SHRT_MAX, -1.0, 1.0);
			gpad_file << frameNo << ":" << thisVal << std::endl;

			frameNo++;
		}
	};

	std::thread t(capture);
	t.detach();

        std::chrono::steady_clock::time_point nowTime;
        while (true)
        {
            std::cout << "loop" << std::endl;
            int lastAccel = 0;
            auto state = gpad.get_state();

            nowTime = std::chrono::steady_clock::now();
            deltaTime += std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastTime).count() / 8.0;
            lastTime = nowTime;

            // this loop updates 8 times a second
            // the naive way

            // steer the car
            double thisVal = map_to_range(state.axes[0], SHRT_MIN, SHRT_MAX, -1.0, 1.0);

            if (thisVal > 0.12 || thisVal < -0.12) {
                steerVal = -thisVal;
            } else {
                steerVal = 0;
            }

            carctl.steer(steerVal);

            int ac = state.axes[5] - lastAccel;
            if (ac >= 5 || ac <= 5)
                carctl.accel(state.axes[5]);
            else if (state.axes[5] <= SHRT_MIN + 20)
                carctl.accel(0);
            lastAccel = state.axes[5];

            // a crude sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_sleep));
        }
    } catch (std::runtime_error e)
    {
        std::cout << "ERR: " << e.what() << std::endl;
        return -2;
    }
}
