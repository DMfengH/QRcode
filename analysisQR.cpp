//
// Created by fenghua on 2022/4/2.
//
#include "iostream"

#include <opencv2/opencv.hpp>
#include <string>
#include <chrono>
#include <Eigen/Core>
#include <Eigen/Dense>

using namespace std;
using namespace cv;
int main(){
    Mat image = imread("../QR.png");
    imshow("QRcode", image);
    Mat image_gray;
    cvtColor(image,image_gray,CV_BGR2GRAY);
    imshow("image_gray", image_gray);
    Mat image_binary;
    threshold(image_gray, image_binary, 145, 255, THRESH_BINARY);
    imshow("image_binary", image_binary);
    waitKey();
}

