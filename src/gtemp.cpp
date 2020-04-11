#include "gtemp.h"

GTemp::GTemp(std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame, std::shared_ptr<QImage> keyFrame, std::shared_ptr<QImage> motionField, std::shared_ptr<QImage> prevOut) :
    Guide(currFrame),
    m_guide(nullptr)
{
    createTemp();
}

GTemp::~GTemp(){
    m_guide = nullptr;
}

std::shared_ptr<QImage> GTemp::getGuide(){
    return m_guide;
}

void GTemp::createTemp(){

}
