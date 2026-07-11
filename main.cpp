#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    cv::CascadeClassifier cascade;
    
    if (!cascade.load("haarcascade_frontalface_default.xml")) {
        std::cerr << "Failed to load XML model.\n";
        return -1;
    }

    cv::VideoCapture cam(0);
    if (!cam.isOpened()) {
        std::cerr << "Cannot open webcam.\n";
        return -1;
    }

    cv::Mat img, gray;

    while (true) {
        cam >> img;
        if (img.empty()) break;

        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        std::vector<cv::Rect> faces;
        cascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(40, 40));

        for (const auto& f : faces) {
            cv::rectangle(img, f, cv::Scalar(0, 255, 0), 3);
            cv::putText(img, "Target Locked", cv::Point(f.x, f.y - 10), 
                        cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("Face Tracker", img);

        if (cv::waitKey(10) == 'q') {
            break;
        }
    }

    return 0;
}
