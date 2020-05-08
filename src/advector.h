#ifndef ADVECTOR_H
#define ADVECTOR_H

#include <opencv2/core/mat.hpp>
#include <Eigen/Dense>
#include <QImage>

class Advector
{
public:
    Advector();

    void advect(const cv::Mat2f& flowField,
                std::shared_ptr<QImage> mask,
                std::shared_ptr<QImage> inFrame,
                std::shared_ptr<QImage> outFrame);

    void advectMask(const cv::Mat2f& flowField,
                    cv::Mat& inMask,
                    cv::Mat& outMask);

private:
    QColor bilinearInterpolate(std::shared_ptr<QImage> img,
                               std::shared_ptr<QImage> mask,
                               cv::Vec2f pos);
    float bilinearInterpolateMask(cv::Mat& img, cv::Vec2f pos);
    bool isInBounds(int x, int y, std::shared_ptr<QImage> img,
                    std::shared_ptr<QImage> mask);
    bool isInBoundsMask(int x, int y, cv::Mat& img);
};

#endif // ADVECTOR_H
