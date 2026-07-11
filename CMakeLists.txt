cmake_minimum_required(VERSION 3.10)
project(FaceTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)

add_executable(face_tracker main.cpp)
target_link_libraries(face_tracker ${OpenCV_LIBS})
