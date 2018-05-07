cmake_minimum_required(VERSION 2.8)

project( i2car )

# TODO: get a better compiler on the pi FFS
set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )
find_package(Threads REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )
file(GLOB SOURCES *.cpp)

add_executable( i2car ${SOURCES} )
target_link_libraries( i2car ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
