# C++ Face Tracker

A live webcam face detection application built in C++ using the OpenCV library. 

## Prerequisites
ensure you have the following installed:
* A **C++17** compatible compiler (GCC, Clang, or MSVC)
* **CMake** (v3.10+)
* **OpenCV** library installed and configured on your system

##  to tun 
1. **Download the AI Model:** 
   Download `haarcascade_frontalface_default.xml` from the [OpenCV GitHub](https://raw.githubusercontent.com/opencv/opencv/master/data/haarcascades/haarcascade_frontalface_default.xml). 
   *Note: Place this `.xml` file in the same directory where you will run the executable.*

2. **Build the Project:**
   Open your terminal in the project directory and run:
   ```bash
   cmake .
   make
