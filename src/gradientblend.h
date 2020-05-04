#ifndef GRADIENTBLEND_H
#define GRADIENTBLEND_H
#include <QString>
#include <opencv2/core.hpp>
class GradientBlender
{
public:
	GradientBlender();
	void blend(const std::vector<QString> &seqA,
		   const std::vector<QString> &seqB,
		   const std::vector<cv::Mat> &errMask,
		   std::vector<cv::Mat3f> &outBlendX,
		   std::vector<cv::Mat3f> &outBlendY);

private:
	void generateGradient(const cv::Mat &img, cv::Mat3f &gradientX, cv::Mat3f &gradientY);
	void assembleMinErrorGradient(const cv::Mat3f &gradXA, const cv::Mat3f &gradYA,
				      const cv::Mat3f &gradXB, const cv::Mat3f &gradYB,
				      const cv::Mat &errMask,
				      cv::Mat3f &gradXMin, cv::Mat3f &gradYMin);
};

#endif // GRADIENTBLEND_H
