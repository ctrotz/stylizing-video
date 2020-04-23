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

private:
    QColor bilinearInterpolate(std::shared_ptr<QImage> img,
                               std::shared_ptr<QImage> mask,
                               cv::Vec2f pos);
    bool isInBounds(int x, int y, std::shared_ptr<QImage> img,
                    std::shared_ptr<QImage> mask);
};

#endif // ADVECTOR_H
