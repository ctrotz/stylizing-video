#ifndef GEDGE_H
#define GEDGE_H

#include "guide.h"
#include <QObject>

class GEdge : public Guide
{
public:
    GEdge(std::shared_ptr<QImage> currFrame);
    virtual ~GEdge();

protected:
    std::shared_ptr<QImage> getGuide();

private:
    std::shared_ptr<QImage> m_guide;
    void createEdge(std::shared_ptr<QImage> currFrame);
};

#endif // GEDGE_H
