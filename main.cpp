#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // 1. Load the pre-trained face detection model
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("haarcascade_frontalface_default.xml")) {
        std::cerr << "Error: Could not load the cascade classifier XML file." << std::endl;
        std::cerr << "Ensure 'haarcascade_frontalface_default.xml' is in the current directory." << std::endl;
        return -1;
    }

    // 2. Open the default system camera (index 0)
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the webcam." << std::endl;
        return -1;
    }

    cv::Mat frame, gray;
    std::cout << "Starting video stream. Press 'ESC' to exit." << std::endl;

    // 3. Process the video stream frame by frame
    while (true) {
        cap >> frame; // Capture a new frame
        if (frame.empty()) {
            std::cerr << "Error: Dropped frame." << std::endl;
            break;
        }

        // Convert to grayscale and equalize histogram for better contrast
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        // Detect faces
        std::vector<cv::Rect> faces;
        // parameters: image, objects, scaleFactor, minNeighbors, flags, minSize
        face_cascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(30, 30));

        // Draw a rectangle around each detected face
        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2); // Blue box
        }

        // Display the live feed
        cv::imshow("C++ Face Tracker", frame);

        // Break the loop if the 'ESC' key (ASCII 27) is pressed
        if (cv::waitKey(10) == 27) {
            break;
        }
    }

    // 4. Clean up
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
