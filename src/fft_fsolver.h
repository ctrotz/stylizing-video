#ifndef FFT_FSOLVER_H
#define FFT_FSOLVER_H
#include "opencv2/core.hpp"
void fourierSolve(cv::Mat3b& imgData, cv::Mat3f& imgGradX, cv::Mat3f& imgGradY, float dataCost);
#endif // FFT_FSOLVER_H
