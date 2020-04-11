#ifndef GEDGE_H
#define GEDGE_H

#include "guide.h"
#include <QObject>

class GEdge : public Guide
{
public:
    GEdge(QImage *currFrame);
    virtual ~GEdge();

protected:
    QImage* getGuide();

private:
    QImage* m_guide;
    void createEdge(QImage *currFrame);
};

#endif // GEDGE_H
