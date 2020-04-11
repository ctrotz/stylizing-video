#ifndef GTEMP_H
#define GTEMP_H

#include "guide.h"
#include <QObject>

class GTemp : public Guide
{
public:
    GTemp(std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame, std::shared_ptr<QImage> keyFrame, std::shared_ptr<QImage> motionField, std::shared_ptr<QImage> prevOut);
    virtual ~GTemp();

protected:
    std::shared_ptr<QImage> getGuide();

private:
    std::shared_ptr<QImage> m_guide;
    void createTemp();
};

#endif // GTEMP_H
