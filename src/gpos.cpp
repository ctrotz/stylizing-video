#include "gpos.h"

#include <QLinearGradient>


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
    std::shared_ptr<QImage> out = std::make_shared<QImage>(width, height, QImage::Format_RGB32);
    out->fill(Qt::white);
    QLinearGradient linearGrad(QPointF(width, 0), QPointF(0, height));
    linearGrad.setColorAt(0, Qt::red);
    linearGrad.setColorAt(1, Qt::green);


}

std::shared_ptr<QImage> GPos::getGuide(){
    return m_guide;
}

std::shared_ptr<QImage> GPos::getMotion(){
    return m_motion;
}

void GPos::createPos(){

}
