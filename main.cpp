#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

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
    std::cout << "Starting video stream..." << std::endl;
    std::cout << "Controls: Press 's' to save a snapshot, 'ESC' to exit." << std::endl;

    double timer;
    double fps;

    // 3. Process the video stream frame by frame
    while (true) {
        timer = (double)cv::getTickCount(); // Start performance timer
        
        cap >> frame; // Capture a new frame
        if (frame.empty()) {
            std::cerr << "Error: Dropped frame." << std::endl;
            break;
        }

        // Convert to grayscale and equalize histogram for better contrast and speed
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        // Detect faces (image, objects, scaleFactor, minNeighbors, flags, minSize)
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(30, 30));

        // Draw a bounding box around each detected face
        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2); // Blue box
        }

        // Calculate and draw the FPS overlay
        fps = cv::getTickFrequency() / ((double)cv::getTickCount() - timer);
        cv::putText(frame, "FPS: " + std::to_string((int)fps), cv::Point(10, 30), 
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

        // Display the live feed
        cv::imshow("C++ Face Tracker", frame);

        // 4. Handle user input
        char key = (char)cv::waitKey(10);
        
        if (key == 27) { 
            // ESC key pressed
            break;
        } 
        else if (key == 's' || key == 'S') {
            // Save snapshot on 's' press
            std::string filename = "snapshot_" + std::to_string(cv::getTickCount()) + ".jpg";
            cv::imwrite(filename, frame);
            std::cout << "📸 Saved: " << filename << std::endl;
        }
    }

    // 5. Clean up
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
