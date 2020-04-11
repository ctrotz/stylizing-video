#ifndef GPOS_H
#define GPOS_H

#include "guide.h"
#include <QObject>
#include <memory>

class GPos : public Guide
{
public:
    GPos(std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame, std::shared_ptr<QImage> g_mask);
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
