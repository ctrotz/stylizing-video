#ifndef FFT_FSOLVER_H
#define FFT_FSOLVER_H
#include "opencv2/core.hpp"
void fourierSolve(cv::Mat3b& imgData, const cv::Mat3f& imgGradX, const cv::Mat3f& imgGradY, float dataCost);
#endif // FFT_FSOLVER_H
