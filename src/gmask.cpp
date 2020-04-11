#include "gmask.h"

GMask::GMask(std::shared_ptr<QImage> currFrame) :
    Guide(currFrame),
    m_mask(nullptr)
{
    createMask(currFrame);
}

GMask::~GMask(){
    m_mask = nullptr;
}

std::shared_ptr<QImage> GMask::getGuide(){
    return m_mask;
}

void GMask::setMask(std::shared_ptr<QImage> mask){
    m_mask = mask;
}

void GMask::createMask(std::shared_ptr<QImage> currFrame){
    m_mask = currFrame; //placeholder
}
