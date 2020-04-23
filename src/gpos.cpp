#include "advector.h"
#include "gpos.h"
#include "opencvutils.h"
#include "opencv2/photo.hpp"

#include <QLinearGradient>
#include <QPainter>

GPos::GPos(std::shared_ptr<QImage> g_mask) :
    Guide(),
    m_guide(generateGradient(g_mask->width(), g_mask->height())),
    m_mask(g_mask)
{

}

GPos::~GPos(){
    m_guide = nullptr;
}

std::shared_ptr<QImage> GPos::generateGradient(int width, int height, QColor xColor, QColor yColor)
{
    std::shared_ptr<QImage> out = std::make_shared<QImage>(width, height, QImage::Format_ARGB32);
    out->fill(Qt::white);
    QLinearGradient linearGrad(QPointF(width, 0), QPointF(0, height));
    linearGrad.setColorAt(0, xColor);
    linearGrad.setColorAt(1, yColor);
    QPainter painter(&(*out));
    painter.fillRect(out->rect(), linearGrad);
    return out;
}

std::shared_ptr<QImage> GPos::getGuide(){
    return m_guide;
}

GPos::GPos(std::shared_ptr<QImage> g_mask, std::shared_ptr<QImage> advected) :
    m_mask(g_mask),
    m_guide(advected)
{

}

GPos GPos::advect(std::shared_ptr<QImage> g_mask, cv::Mat2f& flowField) {
    Advector advector = Advector();
    std::shared_ptr<QImage> advected = std::make_shared<QImage>(m_guide->width(), m_guide->height(), m_guide->format());
    advected->fill(Qt::white);
    advector.advect(flowField, g_mask, m_guide, advected);

    GPos out = GPos(g_mask, advected);
    return out;
}

