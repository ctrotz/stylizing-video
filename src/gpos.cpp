#include "gpos.h"

GPos::GPos(QImage *prevFrame, QImage *currFrame, QImage *g_mask) :
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

QImage* GPos::getGuide(){
    return m_guide;
}

QImage* GPos::getMotion(){
    return m_motion;
}

void GPos::createPos(){

}
