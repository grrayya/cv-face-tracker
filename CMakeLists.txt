cmake_minimum_required(VERSION 3.10)
project(FaceTracker)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the OpenCV installation on your system
find_package(OpenCV REQUIRED)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

# Create the executable from main.cpp
add_executable(face_tracker main.cpp)

# Link the OpenCV libraries to the executable
target_link_libraries(face_tracker ${OpenCV_LIBS})
