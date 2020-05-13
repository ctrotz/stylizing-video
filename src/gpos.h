#ifndef GPOS_H
#define GPOS_H

#include "guide.h"
#include <QObject>
#include <QImage>
#include <opencv4/opencv2/core.hpp>

#include <memory>

class GPos : public Guide
{
public:
    GPos(std::shared_ptr<QImage> g_mask);
    virtual ~GPos();
    static std::shared_ptr<QImage> generateGradient(int width, int height, QColor xColor = Qt::red, QColor yColor = Qt::green);
    void advect(std::shared_ptr<QImage> g_mask, cv::Mat2f& flowField);
    std::shared_ptr<QImage> getGuide() override;
    QString getGuide(int i);
    std::string getType() override;

private:
    std::shared_ptr<QImage> m_guide;
    std::shared_ptr<QImage> m_mask;
};

#endif // GPOS_H
