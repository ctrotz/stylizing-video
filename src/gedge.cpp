#include "gedge.h"

GEdge::GEdge(QImage *currFrame) :
    Guide(currFrame),
    m_guide(nullptr)
{
    createEdge(currFrame);
}

GEdge::~GEdge(){
    m_guide = nullptr;
}

QImage* GEdge::getGuide(){
    return m_guide;
}

void GEdge::createEdge(QImage *currFrame){
    m_guide = currFrame; //placeholder
}
