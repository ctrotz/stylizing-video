#include "gpos.h"

#include <QLinearGradient>
#include <QPainter>


GPos::GPos(std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame, std::shared_ptr<QImage> g_mask) :
    Guide(currFrame),
    m_guide(nullptr),
    m_motion(nullptr)
{
    createPos();
}

GPos::~GPos(){
    m_guide = nullptr;
    m_motion = nullptr;
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

std::shared_ptr<QImage> GPos::getMotion(){
    return m_motion;
}

void GPos::createPos(){

}
