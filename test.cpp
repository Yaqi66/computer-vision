#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // Create a blank black image
    cv::Mat image = cv::Mat::zeros(200, 400, CV_8UC3);

    // Draw some green text
    cv::putText(image, "OpenCV is working!", cv::Point(20, 100),
                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

    // Save the image 
    cv::imwrite("test_output.jpg", image);
    std::cout << "Image successfully saved to test_output.jpg" << std::endl;

    return 0;
}