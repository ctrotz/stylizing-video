#ifndef GTEMP_H
#define GTEMP_H

#include "guide.h"
#include <QObject>

class GTemp : public Guide
{
public:
    GTemp(QImage *prevFrame, QImage *currFrame, QImage *keyFrame, QImage *motionField, QImage *prevOut);
    virtual ~GTemp();

protected:
    QImage* getGuide();

private:
    QImage* m_guide;
    void createTemp();
};

#endif // GTEMP_H
