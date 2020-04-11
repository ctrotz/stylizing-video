#ifndef GMASK_H
#define GMASK_H

#include "guide.h"
#include <QObject>

class GMask : public Guide
{
public:
    GMask(QImage *currFrame);
    virtual ~GMask();

protected:
    QImage* getGuide();
    void setMask(QImage *mask); //for user-provided masks

private:
    QImage* m_mask;
    void createMask(QImage *currFrame);
};

#endif // GMASK_H
