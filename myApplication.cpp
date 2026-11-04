// #include <opencv2/opencv.hpp>
// #include <vector>
// #include <iostream> // Added for std::cerr

// int main() {
//     cv::Mat image = cv::imread("a1/Table2.jpeg");

//     // 1. Check if the image loaded successfully
//     if (image.empty()) {
//         std::cerr << "Error: Could not load image. Check the file path!" << std::endl;
//         return -1;
//     }
//     std::cout << "Image width: " << image.cols << ", height: " << image.rows << std::endl;
//     // 2. Define the 4 corners of the table in the raw photo
//     std::vector<cv::Point2f> srcPts = {
//         cv::Point2f(337, 35),   // Top Left
//         cv::Point2f(1570, 21),  // Top Right
//         cv::Point2f(1888, 451), // Bottom Right
//         cv::Point2f(38, 472)    // Bottom Left
//     };

//     // 3. Define the perfect 1434x683 rectangle we want to create
//     std::vector<cv::Point2f> dstPts = {
//         cv::Point2f(0, 0),          // Top Left
//         cv::Point2f(1434, 0),       // Top Right
//         cv::Point2f(1434, 683),     // Bottom Right
//         cv::Point2f(0, 683)         // Bottom Left
//     };

//     // 4. Generate the transformation matrix
//     cv::Mat warpMatrix = cv::getPerspectiveTransform(srcPts, dstPts);

//     // 5. Create the new, flat, top-down image (where 1 pixel = 1 mm)
//     cv::Mat flatTable;
//     cv::warpPerspective(image, flatTable, warpMatrix, cv::Size(1434, 683));

//     // 6. Show the images on your screen!
//     cv::imshow("Original Image", image);
//     cv::imshow("Flattened Table", flatTable);

//     // Wait until the user presses a key on the keyboard, then exit
//     cv::waitKey(0);
//     return 0;
// }



#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <vector>
#include <iostream> 
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    const char* filename = argc >=2 ? argv[1] : "a1/Table2.jpeg";

    // FIX 1: Removed samples::findFile() so it can read your local file
    Mat src = imread(filename, IMREAD_COLOR);

    // Check if image is loaded fine
    if(src.empty()){
        printf(" Error opening image\n");
        printf(" Program Arguments: [image_name -- default %s] \n", filename);
        return EXIT_FAILURE;
    }

    std::vector<cv::Point2f> srcPts = {
        cv::Point2f(337, 35),   // Top Left
        cv::Point2f(1570, 21),  // Top Right
        cv::Point2f(1888, 451), // Bottom Right
        cv::Point2f(38, 472)    // Bottom Left
    };

    // Define the perfect 1434x683 rectangle we want to create
    std::vector<cv::Point2f> dstPts = {
        cv::Point2f(0, 0),          // Top Left
        cv::Point2f(1434, 0),       // Top Right
        cv::Point2f(1434, 683),     // Bottom Right
        cv::Point2f(0, 683)         // Bottom Left
    };

    // Generate the transformation matrix
    cv::Mat warpMatrix = cv::getPerspectiveTransform(srcPts, dstPts);

    // Create the new, flat, top-down image (where 1 pixel = 1 mm)
    cv::Mat flatTable;
    cv::warpPerspective(src, flatTable, warpMatrix, cv::Size(1434, 683));

    cv::Mat gray, edges;
    cvtColor(flatTable, gray, COLOR_BGR2GRAY);

    cv::Canny(gray, edges, 50, 150);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Loop through every contour found
    for (size_t i = 0; i < contours.size(); i++) {
        
        // CRITICAL: A mathematical ellipse requires at least 5 points to be fitted!
        if (contours[i].size() >= 5) {
            
            // Fit the ellipse to the contour points
            cv::RotatedRect fittedEllipse = cv::fitEllipse(contours[i]);

            // Filter out junk/noise
            float width = fittedEllipse.size.width;
            float height = fittedEllipse.size.height;

            // ASSIGNMENT TIP: We know balls are exactly 43 pixels!
            // Let's only draw ellipses that are between 35 and 50 pixels wide/tall.
            if (width > 35 && width < 130 && height > 35 && height < 130) {
                // Draw the recognized ellipse in red with a thickness of 2
                cv::ellipse(flatTable, fittedEllipse, cv::Scalar(0, 0, 255), 2);
            }
        }
    }

    // FIX 2: Removed the redundant second imshow call
    imshow("Detected Pool Balls", flatTable);
    
    // Check if WSL X-Server is rendering properly by saving a debug image
    imwrite("output_debug.jpg", flatTable);

    waitKey(0);
    return EXIT_SUCCESS;
}