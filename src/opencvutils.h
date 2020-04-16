#ifndef OPENCVUTILS_H
#define OPENCVUTILS_H
// thanks to https://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html for this!!

#include "opencv2/opencv.hpp"
#include <QImage>
using namespace cv;

cv::Mat qimage_to_mat_ref(QImage &img, int format);
QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format);
#endif // OPENCVUTILS_H
