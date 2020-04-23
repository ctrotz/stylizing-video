#ifndef GMASK_H
#define GMASK_H

#include "guide.h"
#include <QObject>
#include <QImage>

class GMask : public Guide
{
public:
    GMask(std::shared_ptr<QImage> currFrame, std::shared_ptr<QImage> currMask = NULL);
    virtual ~GMask();
    std::shared_ptr<QImage> m_mask;

protected:
    std::shared_ptr<QImage> getGuide();
    void setMask(std::shared_ptr<QImage> mask); //for user-provided masks

private:

    void createMask(std::shared_ptr<QImage> currFrame);
};

#endif // GMASK_H
