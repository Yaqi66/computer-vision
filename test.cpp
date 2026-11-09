#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    const vector<string> imageFiles = {
        "a1/Table1.jpeg", "a1/Table2.jpeg", "a1/Table3.jpeg",
        "a1/Table4.jpeg", "a1/Table5.jpeg", "a1/Table6.jpeg"
    };

    int initialIndex = 2; 
    if (argc >= 2) {
        auto it = find(imageFiles.begin(), imageFiles.end(), string(argv[1]));
        if (it != imageFiles.end()) {
            initialIndex = static_cast<int>(it - imageFiles.begin());
        } else {
            cerr << "Warning: " << argv[1] << " is not one of Table1-6.jpeg; "
                << "defaulting to " << imageFiles[initialIndex] << endl;
        }
    }

    Mat src, bgr[3];
    int loadedIndex = -1;

=    auto loadImage = [&](int idx) -> bool {
        Mat loaded = imread(imageFiles[idx], IMREAD_COLOR);
        if (loaded.empty()) {
            cerr << "Error opening image: " << imageFiles[idx] << endl;
            return false;
        }
        src = loaded;
        split(src, bgr);
        for (int i = 0; i < 3; ++i) {
            GaussianBlur(bgr[i], bgr[i], Size(5, 5), 1.5, 1.5);
        }
        return true;
    };

    if (!loadImage(initialIndex)) {
        return EXIT_FAILURE;
    }
    loadedIndex = initialIndex;

    Mat edgesB, edgesG, edgesR, colorEdges;

    const string windowName = "Tune Canny";
    const string circleWindowName = "Circle Detection";
    namedWindow(windowName, WINDOW_NORMAL);
    namedWindow(circleWindowName, WINDOW_NORMAL);

    createTrackbar("Image", windowName, nullptr, static_cast<int>(imageFiles.size()) - 1);
    createTrackbar("Lower", windowName, nullptr, 1020);
    createTrackbar("Upper", windowName, nullptr, 1020);
    createTrackbar("MinArea", windowName, nullptr, 5000);
    createTrackbar("MaxArea", windowName, nullptr, 50000);
    createTrackbar("CircMin", windowName, nullptr, 100);
    setTrackbarPos("Image", windowName, loadedIndex);
    setTrackbarPos("Lower", windowName, 32);
    setTrackbarPos("Upper", windowName, 99);
    setTrackbarPos("MinArea", windowName, 687);
    setTrackbarPos("MaxArea", windowName, 5344);
    setTrackbarPos("CircMin", windowName, 80);

    cout << "Adjust the sliders (Image 0-5 selects Table1-6.jpeg). "
        << "Press Enter to use the settings, or Esc to quit." << endl;

    while (true) {
        int imageIndex = getTrackbarPos("Image", windowName);
        if (imageIndex != loadedIndex) {
            if (loadImage(imageIndex)) {
                loadedIndex = imageIndex;
                cout << "Switched to " << imageFiles[loadedIndex] << endl;
            } else {
                setTrackbarPos("Image", windowName, loadedIndex);
            }
        }

        int lower = getTrackbarPos("Lower", windowName);
        int upper = getTrackbarPos("Upper", windowName);

        if (lower > upper) {
            std::swap(lower, upper);
        }

        Canny(bgr[0], edgesB, lower, upper);
        Canny(bgr[1], edgesG, lower, upper);
        Canny(bgr[2], edgesR, lower, upper);

        bitwise_or(edgesB, edgesG, colorEdges);
        bitwise_or(colorEdges, edgesR, colorEdges);

        int minArea = getTrackbarPos("MinArea", windowName);
        int maxArea = getTrackbarPos("MaxArea", windowName);
        double circThresh = getTrackbarPos("CircMin", windowName) / 100.0;

        Mat closedEdges;
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        morphologyEx(colorEdges, closedEdges, MORPH_CLOSE, kernel, Point(-1, -1), 2);

        vector<vector<Point>> contours;
        findContours(closedEdges, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);


        Mat circleDisplay = src.clone();
        int detectedCount = 0;
        for (const auto& c : contours) {

            vector<Point> hull;
            convexHull(c, hull);

            double area = contourArea(hull);
            double perimeter = arcLength(hull, true);
            if (perimeter <= 0 || area < minArea || area > maxArea) {
                continue;
            }

            double circularity = 4 * CV_PI * area / (perimeter * perimeter);
            if (circularity < circThresh) {
                continue;
            }

            Point2f center;
            float radius;
            minEnclosingCircle(hull, center, radius);
            circle(circleDisplay, center, cvRound(radius), Scalar(0, 0, 255), 2);
            circle(circleDisplay, center, 2, Scalar(0, 255, 0), 3);
            ++detectedCount;
        }

        imshow(windowName, colorEdges);
        imshow(circleWindowName, circleDisplay);

        int key = waitKey(30);
        if (key == 13 || key == 10) {
            cout << "Using image: " << imageFiles[loadedIndex] << endl;
            cout << "Using Canny thresholds: " << lower << ", " << upper << endl;
            cout << "Detected " << detectedCount << " ball(s)." << endl;
            break;
        }
        if (key == 27 || getWindowProperty(windowName, WND_PROP_AUTOSIZE) < 0) {
            return EXIT_SUCCESS;
        }
    }

    destroyWindow(windowName);
    destroyWindow(circleWindowName);

    return 0;
}