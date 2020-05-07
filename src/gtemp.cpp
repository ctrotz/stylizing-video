#include "gtemp.h"
#include "advector.h"
#include <iostream>

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
    updateGuide(prevStylizedFrame, motionField, mask);
}

GTemp::~GTemp()
{
    m_guide = nullptr;
}

QString GTemp::getGuide(int i)
{
    QString filename("./guides/temp");
    filename.append(QString::number(i));
    filename.append(".png");
    m_guide->save(filename, nullptr, 100);
    return filename;
}

// Recompute guide given mask, optical flow field, and previous stylized frame
void GTemp::updateGuide(std::shared_ptr<QImage> prevStylizedFrame,
                        cv::Mat& motionField,
                        std::shared_ptr<QImage> mask)
{
    std::shared_ptr<QImage> newFrame(new QImage(*prevStylizedFrame));
//    std::cout << newFrame->height() << " " << newFrame->width() << std::endl;
    m_advector.advect(motionField, mask, prevStylizedFrame, newFrame);
    m_guide = newFrame;
}

