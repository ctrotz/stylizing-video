#ifndef GRADIENTBLEND_H
#define GRADIENTBLEND_H
#include <QString>
#include <opencv2/core.hpp>
#include "iohandler.h"
class GradientBlender
{
public:
	GradientBlender();
    void blend(Sequence& a, Sequence& b,
		   const std::vector<cv::Mat> &errMask,
		   std::vector<cv::Mat3f> &outBlendX,
		   std::vector<cv::Mat3f> &outBlendY);

	void generateGradient(const cv::Mat &img, cv::Mat3f &gradientX, cv::Mat3f &gradientY);
private:
	void assembleMinErrorGradient(const cv::Mat3f &gradXA, const cv::Mat3f &gradYA,
				      const cv::Mat3f &gradXB, const cv::Mat3f &gradYB,
				      const cv::Mat &errMask,
				      cv::Mat3f &gradXMin, cv::Mat3f &gradYMin);
};

#endif // GRADIENTBLEND_H
