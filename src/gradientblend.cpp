#include "gradientblend.h"
#include "opencv2/core.hpp"
#include "opencv2/core/base.hpp"
#include "opencv2/core/hal/interface.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencvutils.h"
#include <iostream>

GradientBlender::GradientBlender() {}

void GradientBlender::blend(const std::vector<QString> &seqA, const std::vector<QString> &seqB, const std::vector<cv::Mat> &errMask, std::vector<cv::Mat3f> &outBlendX, std::vector<cv::Mat3f> &outBlendY)
{
	assert(seqA.size() == seqB.size());
	assert(seqA.size() == errMask.size());
	for (uint i = 0; i < seqA.size(); ++i) {
		// Read in imgs
		cv::Mat Oai = imread(seqA.at(i).toStdString(), cv::IMREAD_COLOR);
		cv::Mat Obi = imread(seqB.at(i).toStdString(), cv::IMREAD_COLOR);
		
		std::cout << "images read in" << std::endl;
        	assert(Oai.channels() == 3 && Obi.channels() == 3);
		cv::Mat3f gradXA, gradYA, gradXB, gradYB;
		generateGradient(Oai, gradXA, gradYA);	
		generateGradient(Obi, gradXB, gradYB);
		std::cout << "gradient generated" << std::endl;
		cv::Mat3f gradXMin, gradYMin;
		assembleMinErrorGradient(gradXA, gradYA, gradXB, gradYB, errMask.at(i), gradXMin, gradYMin);
		outBlendX.push_back(gradXMin);
		outBlendY.push_back(gradYMin);
		std::cout << "main task done, time to imshow" << std::endl;

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
        double min,max;
        cv::minMaxLoc(gradX.at(i),& min, &max);
        std::cout << "min: " << std::to_string(min) << " max: " << std::to_string(max) << std::endl;
	}


	std::cout << "made it past sobel" << std::endl;
	cv::merge(gradX, gradientX);
	cv::merge(gradY, gradientY);
	std::cout << "made it past merge" << std::endl;

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
