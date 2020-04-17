#ifndef SIMPLEFLOW_H
#define SIMPLEFLOW_H
#include "opencv2/optflow.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <cstdio>
#include <iostream>

using namespace cv;
Mat calculateFlow(const Mat& i1, const Mat& i2, bool retColor = false,  bool imshowFlag = false) {

    namedWindow("flow");
    Mat flow;

    if (i1.empty()) {
      printf("Image #1 cannot be read\n");
      return Mat();
    }

    if (i2.empty()) {
      printf("Image #2 cannot be read\n");
      return Mat();
    }

    if (i1.rows != i2.rows && i1.cols != i2.cols) {
      printf("Images should be of equal sizes\n");
      return Mat();
    }

    if (i1.type() != 16 || i2.type() != 16) {
      printf("Images should be of equal type CV_8UC3\n");
      return Mat();
    }
    optflow::calcOpticalFlowSF(i1, i2,
                        flow,
                        3, 2, 4, 4.1, 25.5, 18, 55.0, 25.5, 0.35, 18, 55.0, 25.5, 10);

    Mat xy[2];
    split(flow, xy);

    //calculate angle and magnitude
    Mat magnitude, angle;
    cartToPolar(xy[0], xy[1], magnitude, angle, true);

    //translate magnitude to range [0;1]
    double mag_max;
    minMaxLoc(magnitude, 0, &mag_max);
    magnitude.convertTo(magnitude, -1, 1.0/mag_max);

    //build hsv image
    Mat _hsv[3], hsv;
    _hsv[0] = angle;
    _hsv[1] = Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magnitude;
    merge(_hsv, 3, hsv);

    //convert to BGR and show
    Mat bgr;//CV_32FC3 matrix
    cvtColor(hsv, bgr, COLOR_HSV2BGR);
    if (imshowFlag) {
        imshow("flow", bgr);
        waitKey(0);
    }
    if (retColor) {
        return bgr;
    } else {
        return flow;
    }
}

#endif // SIMPLEFLOW_H
