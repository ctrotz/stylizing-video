#include "gpos.h"

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

std::shared_ptr<QImage> GPos::getGuide(){
    return m_guide;
}

std::shared_ptr<QImage> GPos::getMotion(){
    return m_motion;
}

void GPos::createPos(){

}
