#include "gedge.h"

GEdge::GEdge(std::shared_ptr<QImage> currFrame) :
    Guide(currFrame),
    m_guide(nullptr)
{
    createEdge(currFrame);
}

GEdge::~GEdge(){
    m_guide = nullptr;
}

std::shared_ptr<QImage> GEdge::getGuide(){
    return m_guide;
}

void GEdge::createEdge(std::shared_ptr<QImage> currFrame){
    m_guide = currFrame; //placeholder
}
