#ifndef HISTOGRAMBLEND_H
#define HISTOGRAMBLEND_H

#include <Eigen/Dense>
#include <QImage>
#include <opencv2/core/mat.hpp>

class HistogramBlender
{
public:
    HistogramBlender();

    void blend(const std::vector<QString> &seqA,
               const std::vector<QString> &seqB,
               const std::vector<cv::Mat1f> &errMask,
               std::vector<std::shared_ptr<QImage>> &outBlend);

private:
    void computeMeanAndStd(const cv::Mat &img, float *mean, float *std);
    void assembleMinErrorImg(const cv::Mat &Oai, const cv::Mat &Obi,
                             const cv::Mat1f &errMask, cv::Mat &minErrorImg);
    void histogramTransform(const cv::Mat &inputImg, float *inputMean, float *inputStd,
                            float *refMean, float *refStd, cv::Mat &outputImg);
};

#endif // HISTOGRAMBLEND_H
