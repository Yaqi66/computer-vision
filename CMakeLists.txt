cmake_minimum_required(VERSION 3.1)
project(OpenCVTest)

find_package(OpenCV REQUIRED)

add_executable(a1 myApplication.cpp)
target_link_libraries(a1 ${OpenCV_LIBS})
