#include "histogramblend.h"
#include "opencvutils.h"

#include <Eigen/Dense>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>

using namespace cv;

HistogramBlender::HistogramBlender() {}

void HistogramBlender::blend(const std::vector<std::shared_ptr<QString>> &seqA,
                 const std::vector<std::shared_ptr<QString>> &seqB,
                 const std::vector<Mat1f> &errMask,
                 std::vector<std::shared_ptr<QImage>> &outBlend)
{
    for (uint i = 0; i < seqA.size(); ++i) {
        // Read in images
        Mat Oai_bgr = imread(seqA.at(i)->toStdString(), cv::IMREAD_COLOR);
        Mat Obi_bgr = imread(seqB.at(i)->toStdString(), cv::IMREAD_COLOR);

        // Convert to Lab color space
        Mat Oai_lab(Oai_bgr);
        Mat Obi_lab(Obi_bgr);
        cvtColor(Oai_bgr, Oai_lab, COLOR_BGR2Lab);
        cvtColor(Obi_bgr, Obi_lab, COLOR_BGR2Lab);

        // Assemble minimum error image
        Mat minErrorImg(Oai_lab);
        assembleMinErrorImg(Oai_lab, Obi_lab, errMask.at(i), minErrorImg);

        // Compute mean and std of images
        float Oai_mean[3];
        float Oai_std[3];
        computeMeanAndStd(Oai_lab, Oai_mean, Oai_std);

        float Obi_mean[3];
        float Obi_std[3];
        computeMeanAndStd(Obi_lab, Obi_mean, Obi_std);

        float minError_mean[3];
        float minError_std[3];
        computeMeanAndStd(minErrorImg, minError_mean, minError_std);

        // 'Gaussianization' mean and std (section 4.2)
        float t_meanVal = 0.5f * 256.f;
        float t_stdVal = (1.f / 36.f) * 256.f;
        float t_mean[3] = {t_meanVal, t_meanVal, t_meanVal};
        float t_std[3] = {t_stdVal, t_stdVal, t_stdVal};

        // Gaussianize Oai and Oab
        Mat Oai_t(Oai_lab);
        Mat Obi_t(Obi_lab);
        histogramTransform(Oai_lab, Oai_mean, Oai_std, t_mean, t_std, Oai_t);
        histogramTransform(Obi_lab, Obi_mean, Obi_std, t_mean, t_std, Obi_t);

        // Histogram-preserving blending operation (section 3.3)
        Mat Oabi_t = (((0.5f * (Oai_t + Obi_t)) - t_meanVal) / 0.5f) + t_meanVal;

        // Transform back to minError histogram
        Mat Oabi(Oabi_t);
        float Oabi_t_mean[3];
        float Oabi_t_std[3];
        computeMeanAndStd(Oabi_t, Oabi_t_mean, Oabi_t_std);
        histogramTransform(Oabi_t, Oabi_t_mean, Oabi_t_std, minError_mean, minError_std, Oabi);

        // Covert to RGB
        Mat Oabi_rgb(Oabi);
        cvtColor(Oabi, Oabi_rgb, COLOR_Lab2RGB);

        outBlend.push_back(
                std::shared_ptr<QImage>(
                    new QImage(mat_to_qimage_ref(Oabi, QImage::Format_RGB16))));
    }
}

void HistogramBlender::computeMeanAndStd(const Mat &img, float *mean, float *std)
{
    int sum[3] = {0, 0, 0};
    int n = img.rows * img.cols;

    // Calculate mean
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
		Vec3b curPixel = img.at<Vec3b>(i, j);

		for (int k = 0; k < 3; ++k) {
			sum[k] += curPixel[k];
	    }
        }
    }
    mean[0] = sum[0] / n;
    mean[1] = sum[1] / n;
    mean[2] = sum[2] / n;

    // Zero out sums
    sum[0] = 0;
    sum[1] = 0;
    sum[2] = 0;

    // Calculate std
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            Vec3b curPixel = img.at<Vec3b>(i, j);

            for (int k = 0; k < 3; ++k) {
                int temp = curPixel[k] - mean[k];
                sum[k] += temp * temp;
            }
        }
    }
    std[0] = sqrt(sum[0] / n);
    std[1] = sqrt(sum[1] / n);
    std[2] = sqrt(sum[2] / n);
}

void HistogramBlender::assembleMinErrorImg(const Mat &Oai, const Mat &Obi,
                                           const Mat1f &errMask, Mat &minErrorImg)
{
    for (int i = 0; i < Oai.rows; ++i) {
        for (int j = 0; j < Oai.cols; ++j) {
            if (errMask.at<float>(i, j) == 1.f) {
                minErrorImg.at<Vec3b>(i, j) = Oai.at<Vec3b>(i, j);
            } else {
                minErrorImg.at<Vec3b>(i, j) = Obi.at<Vec3b>(i, j);
            }
        }
    }
}

// Method derived from Reinhard et al. (2001) "Color Transfer between Images"
void HistogramBlender::histogramTransform(const Mat &inputImg, float *inputMean, float *inputStd,
                                          float *refMean, float *refStd, Mat &outputImg)
{
    for (int i = 0; i < inputImg.rows; ++i) {
        for (int j = 0; j < inputImg.cols; ++j) {
            Vec3b inputPixel = inputImg.at<Vec3b>(i, j);
            Vec3b outPixel = Vec3b();

            for (int k = 0; k < 3; ++k) {
                float x = inputPixel[k];
                x -= inputMean[k];
                x *= refStd[k] / inputStd[k];
                x += refMean[k];
                outPixel[k] = static_cast<int>(round(x));
            }

            outputImg.at<Vec3b>(i, j) = outPixel;
        }
    }
}
