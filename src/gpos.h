#ifndef GPOS_H
#define GPOS_H

#include "guide.h"
#include <QObject>

class GPos : public Guide
{
public:
    GPos(QImage *prevFrame, QImage *currFrame, QImage *g_mask);
    virtual ~GPos();

protected:
    QImage* getGuide();
    QImage* getMotion(); //for G_temp

private:
    QImage* m_guide;
    QImage* m_motion;

    void createPos();
};

#endif // GPOS_H
