// #include "opencv2/imgcodecs.hpp"
// #include "opencv2/highgui.hpp"
// #include "opencv2/imgproc.hpp"
// #include <vector>
// #include <iostream> 
// #include <opencv2/opencv.hpp>

// using namespace cv;
// using namespace std;


// int main(int argc, char** argv)
// {
//     const char* filename = argc >=2 ? argv[1] : "a1/Table2.jpeg";

//     Mat src = imread(filename, IMREAD_COLOR);

//     // Check if image is loaded fine
//     if(src.empty()){
//         printf(" Error opening image\n");
//         printf(" Program Arguments: [image_name -- default %s] \n", filename);
//         return EXIT_FAILURE;
//     }

//     std::vector<cv::Point2f> srcPts = {
//         cv::Point2f(337, 35),   // Top Left
//         cv::Point2f(1570, 21),  // Top Right
//         cv::Point2f(1888, 451), // Bottom Right
//         cv::Point2f(38, 472)    // Bottom Left
//     };

//     double widthTop = norm(srcPts[1]-srcPts[0]);
//     double widthBottom = norm(srcPts[2]-srcPts[1]);

//     double heightRight = norm(srcPts[2]-srcPts[1]);
//     double heightLeft = norm(srcPts[3]-srcPts[1]);

//     // std::vector<cv::Point2f> dstPts = {
//     //     cv::Point2f(0, 0),          // Top Left
//     //     cv::Point2f(widthTop-1, 0),       // Top Right
//     //     cv::Point2f(widthBottom-1, heightRight-1),     // Bottom Right
//     //     cv::Point2f(0, heightLeft-1)         // Bottom Left
//     // };
//     std::vector<cv::Point2f> dstPts = {
//         cv::Point2f(0, 0),          // Top Left
//         cv::Point2f(1434, 0),       // Top Right
//         cv::Point2f(1434, 683),     // Bottom Right
//         cv::Point2f(0, 683)         // Bottom Left
//     };

//     cv::Mat warpMatrix = cv::getPerspectiveTransform(srcPts, dstPts);

//     cv::Mat flatTable;
//     cv::warpPerspective(src, flatTable, warpMatrix, cv::Size(1434, 683));

//     cv::Mat gray, edges;
//     //cvtColor(flatTable, gray, COLOR_BGR2GRAY);
//     cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
//     cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);

//     //cv::Canny(gray, edges, 50, 150);

//     //std::vector<std::vector<cv::Point>> contours;
//     //cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

//     std::vector<cv::Vec3f> circles;

//     cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 
//                     1,
//                     15,
//                     300,        
//                     15,         
//                      1, 25);  

//     for (size_t i = 0; i < circles.size(); i++) {
//         cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//         int radius = cvRound(circles[i][2]);
        
//         // Draw the circle center
//         cv::circle(src, center, 3, cv::Scalar(0, 255, 0), -1, cv::LINE_AA);

//         // Draw the circle outline
//         cv::circle(src, center, radius, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
//     }

//     // for (size_t i = 0; i < contours.size(); i++) {
        
//     //     if (contours[i].size() >= 5) {
            
//     //         cv::RotatedRect fittedEllipse = cv::fitEllipse(contours[i]);

//     //         float width = fittedEllipse.size.width;
//     //         float height = fittedEllipse.size.height;

//     //         if (width > 35 && width < 130 && height > 35 && height < 130) {
//     //             cv::ellipse(flatTable, fittedEllipse, cv::Scalar(0, 0, 255), 2);
//     //         }
//     //     }
//     // }
//     std::cout << "Reaching the end!" << std::endl;
//     cv::imwrite("output.jpg", src);
//     imshow("Detected Pool Balls", src);
//     std::cout << "Reaching the end!" << std::endl;
//     waitKey(0);
//     return EXIT_SUCCESS;
// }

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    const char* filename = argc >= 2 ? argv[1] : "a1/Table2.jpeg";
    Mat src = imread(filename, IMREAD_COLOR);

    if (src.empty()) {
        cerr << "Error: Could not open or find the image: " << filename << endl;
        return EXIT_FAILURE;
    }

    // Color-Preserving Canny 
    Mat bgr[3];
    split(src, bgr); 

    Mat edgesB, edgesG, edgesR, colorEdges;
    
    // Blur each channel individually
    GaussianBlur(bgr[0], bgr[0], Size(5, 5), 1.5, 1.5);
    GaussianBlur(bgr[1], bgr[1], Size(5, 5), 1.5, 1.5);
    GaussianBlur(bgr[2], bgr[2], Size(5, 5), 1.5, 1.5);

    // Run Canny on each channel
    Canny(bgr[0], edgesB, 50, 150);
    Canny(bgr[1], edgesG, 50, 150);
    Canny(bgr[2], edgesR, 50, 150);

    bitwise_or(edgesB, edgesG, colorEdges);
    bitwise_or(colorEdges, edgesR, colorEdges);


    imwrite("edges_method2_color.jpg", colorEdges);
    cout << "Successfully saved edge maps to your current directory!" << endl;

    imshow("Original Image", src);
    imshow("Color-Split Canny", colorEdges);

    waitKey(0);
    return EXIT_SUCCESS;
}