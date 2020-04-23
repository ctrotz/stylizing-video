#ifndef GPOS_H
#define GPOS_H

#include "guide.h"
#include <QObject>
#include <QImage>
#include <opencv2/core.hpp>

#include <memory>

class GPos : public Guide
{
public:
    GPos(std::shared_ptr<QImage> g_mask);
    virtual ~GPos();
    static std::shared_ptr<QImage> generateGradient(int width, int height, QColor xColor = Qt::red, QColor yColor = Qt::green);
    GPos advect(std::shared_ptr<QImage> g_mask, cv::Mat2f& flowField);
    #ifdef QT_DEBUG
    std::shared_ptr<QImage> getGuide();
#else
protected:
    std::shared_ptr<QImage> getGuide();
#endif
private:

    GPos(std::shared_ptr<QImage> g_mask, std::shared_ptr<QImage> advected);
    std::shared_ptr<QImage> m_guide;
    std::shared_ptr<QImage> m_mask;
};

#endif // GPOS_H
