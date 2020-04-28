#ifndef GTEMP_H
#define GTEMP_H

#include "guide.h"
#include "advector.h"

#include <QObject>

#include <opencv2/core/mat.hpp>

class GTemp : public Guide
{
public:
    GTemp();
    GTemp(std::shared_ptr<QImage> prevStylizedFrame,
          cv::Mat& motionField,
          std::shared_ptr<QImage> mask);

    virtual ~GTemp();

    QString getGuide(int i);

    void updateGuide(std::shared_ptr<QImage> prevStylizedFrame,
                     cv::Mat& motionField,
                     std::shared_ptr<QImage> mask);

private:
    std::shared_ptr<QImage> m_guide;
    Advector m_advector;
};

#endif // GTEMP_H
