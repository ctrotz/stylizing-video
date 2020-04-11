#include "gmask.h"

GMask::GMask(QImage *currFrame) :
    Guide(currFrame),
    m_mask(nullptr)
{
    createMask(currFrame);
}

GMask::~GMask(){
    m_mask = nullptr;
}

QImage* GMask::getGuide(){
    return m_mask;
}

void GMask::setMask(QImage *mask){
    m_mask = mask;
}

void GMask::createMask(QImage *currFrame){
    m_mask = currFrame; //placeholder
}
