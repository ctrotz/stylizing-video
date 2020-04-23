#include "gtemp.h"
#include "advector.h"

GTemp::GTemp() :
    Guide(nullptr),
    m_guide(nullptr),
    m_advector()
{
}

GTemp::GTemp(std::shared_ptr<QImage> prevStylizedFrame,
             cv::Mat& motionField,
             std::shared_ptr<QImage> mask) :
    Guide(nullptr),
    m_guide(nullptr),
    m_advector()
{
    updateGuide(mask, motionField, prevStylizedFrame);
}

GTemp::~GTemp()
{
    m_guide = nullptr;
}

std::shared_ptr<QImage> GTemp::getGuide()
{
    return m_guide;
}

// Recompute guide given mask, optical flow field, and previous stylized frame
void GTemp::updateGuide(std::shared_ptr<QImage> prevStylizedFrame,
                        cv::Mat& motionField,
                        std::shared_ptr<QImage> mask)
{
    std::shared_ptr<QImage> newFrame(new QImage(*prevStylizedFrame));
    m_advector.advect(motionField, mask, prevStylizedFrame, newFrame);
    m_guide = newFrame;
}

