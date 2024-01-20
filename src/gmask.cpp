#include "gmask.h"

#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include "opencvutils.h"

using namespace std;
using namespace cv;

GMask::GMask(std::shared_ptr<QImage> currFrame, std::shared_ptr<QImage> currMask) :
    Guide(currFrame)
//    m_mask(nullptr)
{
//    if (currMask == NULL) {
        createMask(currFrame, 0);
//    } else {
//        m_mask = currMask;
//    }
}

GMask::~GMask(){
    m_mask = nullptr;
}

void GMask::updateFrame(std::shared_ptr<QImage> frame, int i) {
    createMask(frame, i);
}

string GMask::getType()
{
    return "mask";
}

std::shared_ptr<QImage> GMask::getGuide()
{
    return m_mask;
}

void GMask::setMask(std::shared_ptr<QImage> mask){
//    m_mask = mask;
}

void GMask::createMask(std::shared_ptr<QImage> currFrame, int i){

    // Detect edges
    Mat mat = qimage_to_mat_ref(*currFrame);

    // First, blur
    Mat blurMat(mat.size(), CV_8UC3);
    blur(mat, blurMat, Size(10,10));

    // Then use canny to extract edges
    Mat cannyOut(mat.size(), CV_8UC1);
    morphologyEx(mat,cannyOut,MORPH_CLOSE, Mat::ones(6,6,CV_8U),Point(-1,-1),5);
    cvtColor(cannyOut,cannyOut,COLOR_RGB2GRAY);
    Canny(cannyOut,cannyOut,50,50);
    dilate(cannyOut,cannyOut,Mat::ones(5,5,CV_8U),Point(-1,-1),3);
    erode(cannyOut,cannyOut,Mat::ones(5,5,CV_8U),Point(-1,-1),1);
    morphologyEx(cannyOut,cannyOut,MORPH_CLOSE, Mat::ones(6,6,CV_8U),Point(-1,-1),5);

    // Find contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(cannyOut,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,cv::Point2i(0,0));

    // Draw contours
    Mat markers(mat.size(), CV_8UC1);
    markers = Scalar::all(0);

    int compCount = 0;
    int minSize = 100;
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0], compCount++ ) {
        if (contours[idx].size() < minSize) {
            continue;
        }
        drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
    }

    if( compCount == 0 ) {
        return;
    }
    cv::normalize(markers,markers,0,255,NORM_MINMAX);

    // Create mask by tossing values below 0.5f in the markers
    // These are smaller
    Mat mask(markers.size(),CV_8U);
    markers.convertTo(markers,CV_32FC1,1);
    for (int y = 0; y < markers.rows; y++) {
        for (int x = 0; x < markers.cols; x++) {
            float val = markers.at<float>(y,x)/255.f;
            if (val > 0.5f) {
                mask.at<uchar>(y,x) = GC_FGD;
            } else {
                mask.at<uchar>(y,x) = GC_BGD;
            }
        }
    }

    // Then call grab cut
    Mat fgdModel, bgdModel;
    Rect rect = Rect(0,0,currFrame->width(),currFrame->height());
    grabCut(mat,mask,rect,bgdModel,fgdModel,5,GC_INIT_WITH_MASK);
    mask *= 255;

    for (int j = 0; j < 2; j++) {
        findContours(mask,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,cv::Point2i(0,0));
        for (int i = 0; i < contours.size(); i++) {
            fillConvexPoly(mask,contours[i],255);
        }
    }

    // Make sure mask is 1 channel
    mask.convertTo(mask,CV_8UC1,1);
    QImage imgIn = QImage(mask.data, mask.cols, mask.rows, static_cast<int>(mask.step), QImage::Format_Grayscale8);
    m_mask = make_shared<QImage>(imgIn);

    imgIn.save("./imgIn.jpg");
}
