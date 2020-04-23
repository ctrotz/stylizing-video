#include "gtemp.h"
#include "advector.h"

GTemp::GTemp(std::shared_ptr<QImage> currFrame,
             std::shared_ptr<QImage> mask,
             cv::Mat& motionField,
             std::shared_ptr<QImage> prevOut) :
    Guide(currFrame),
    m_guide(nullptr)
{
    createTemp(motionField, mask, prevOut);
}

GTemp::~GTemp()
{
    m_guide = nullptr;
}

std::shared_ptr<QImage> GTemp::getGuide()
{
    return m_guide;
}

void GTemp::createTemp(cv::Mat& motionField, std::shared_ptr<QImage> mask,
                       std::shared_ptr<QImage> prevFrame)
{
    Advector advector = Advector();
    std::shared_ptr<QImage> newFrame(new QImage(*prevFrame));
    advector.advect(motionField, mask, prevFrame, newFrame);
    m_guide = newFrame;
}
