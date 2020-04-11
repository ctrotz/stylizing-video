#include "gtemp.h"

GTemp::GTemp(QImage *prevFrame, QImage *currFrame, QImage *keyFrame, QImage *motionField, QImage *prevOut) :
    Guide(currFrame),
    m_guide(nullptr)
{
    createTemp();
}

GTemp::~GTemp(){
    m_guide = nullptr;
}

QImage* GTemp::getGuide(){
    return m_guide;
}

void GTemp::createTemp(){

}
