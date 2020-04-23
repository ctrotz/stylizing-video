#ifndef GTEMP_H
#define GTEMP_H

#include "guide.h"
#include <QObject>
#include <opencv2/core/mat.hpp>

class GTemp : public Guide
{
public:
    GTemp(std::shared_ptr<QImage> currFrame,
          std::shared_ptr<QImage> mask,
          cv::Mat& motionField,
          std::shared_ptr<QImage> prevOut);
    virtual ~GTemp();
    std::shared_ptr<QImage> getGuide();

private:
    std::shared_ptr<QImage> m_guide;
    void createTemp(cv::Mat& motionField, std::shared_ptr<QImage> mask,
                    std::shared_ptr<QImage> prevFrame);
};

#endif // GTEMP_H
