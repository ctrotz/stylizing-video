#include "gradientblend.h"
#include "opencv2/core.hpp"
#include "opencv2/core/base.hpp"
#include "opencv2/core/hal/interface.h"
#include "opencv2/core/types.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencvutils.h"
#include <iostream>

GradientBlender::GradientBlender() {}

void GradientBlender::blend(Sequence& a, Sequence& b, const std::vector<cv::Mat> &errMask, std::vector<cv::Mat3f> &outBlendX, std::vector<cv::Mat3f> &outBlendY)
{
    for (uint i = 0; i <= a.size; i++) {
		// Read in imgs
        cv::Mat Oai = imread(IOHandler::getOutputPath(a, i), cv::IMREAD_COLOR);
        cv::Mat Obi = imread(IOHandler::getOutputPath(b, i), cv::IMREAD_COLOR);
		
        assert(Oai.channels() == 3 && Obi.channels() == 3);
		cv::Mat3f gradXA, gradYA, gradXB, gradYB;
		generateGradient(Oai, gradXA, gradYA);	
		generateGradient(Obi, gradXB, gradYB);
		cv::Mat3f gradXMin, gradYMin;
		assembleMinErrorGradient(gradXA, gradYA, gradXB, gradYB, errMask.at(i), gradXMin, gradYMin);
		outBlendX.push_back(gradXMin);
		outBlendY.push_back(gradYMin);

		//// DEBUG 
		//std::vector<Mat1f> gx, gy;
		//cv::split(gradXMin, gx);
		//cv::split(gradYMin, gy);
		//for (uint i = 0; i < gx.size(); ++i) {
		//	cv::imshow("gradX", gx.at(i));
		//	cv::imshow("gradY", gy.at(i));
		//}
		//if (cv::waitKey() == 27) {
		//	cv::destroyAllWindows();
		//}
	}	

}

void GradientBlender::generateGradient(const cv::Mat &img, cv::Mat3f &gradientX, cv::Mat3f &gradientY) 
{
	assert(img.channels() == 3);
	std::vector<cv::Mat> bgrChannels(3);
	std::vector<cv::Mat> gradX(3);
	std::vector<cv::Mat> gradY(3);
	cv::split(img, bgrChannels);
	for (uint i = 0; i < bgrChannels.size(); ++i) {
        cv::Sobel(bgrChannels.at(i), gradX.at(i), CV_32F, 1, 0, 3, 1/8.f, 0, cv::BORDER_REFLECT);
        cv::Sobel(bgrChannels.at(i), gradY.at(i), CV_32F, 0, 1, 3, 1/8.f, 0, cv::BORDER_REFLECT);

//        cv::Mat xdisp, ydisp;
//	cv::convertScaleAbs(gradX.at(i), xdisp);
//	cv::convertScaleAbs(gradY.at(i), ydisp);
//
//	cv::copyMakeBorder(xdisp, xdisp, 50, 50, 50, 50, cv::BORDER_CONSTANT, Scalar(0, 0, 0));	
//	cv::copyMakeBorder(ydisp, ydisp, 50, 50, 50, 50, cv::BORDER_CONSTANT, Scalar(0, 0, 0));
//	
//	cv::imshow("gx", xdisp);
//	cv::imshow("gy", ydisp);
//	cv::waitKey(0);
//
        double min,max;
        cv::minMaxLoc(gradX.at(i),& min, &max);
	}


	cv::merge(gradX, gradientX);
	cv::merge(gradY, gradientY);

}

void GradientBlender::assembleMinErrorGradient(const cv::Mat3f &gradXA, const cv::Mat3f &gradYA,
				      const cv::Mat3f &gradXB, const cv::Mat3f &gradYB, 
				      const cv::Mat &errMask,
				      cv::Mat3f &gradXMin, cv::Mat3f &gradYMin)
{
	gradXMin = cv::Mat::zeros(gradXA.size(), gradXA.type());
	gradYMin = cv::Mat::zeros(gradYA.size(), gradYA.type());

    // copy O_b's gradient in using mask
	gradXB.copyTo(gradXMin, errMask);
	gradYB.copyTo(gradYMin, errMask);	

    // flip the mask
    cv::Mat flipErrMask = cv::Mat::ones(errMask.size(), errMask.type()) - errMask;

    // copy O_a's gradient in using mask
    gradXA.copyTo(gradXMin, flipErrMask);
    gradYA.copyTo(gradYMin, flipErrMask);
}
