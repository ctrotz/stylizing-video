#ifndef GMASK_H
#define GMASK_H

#include "guide.h"
#include <QObject>

class GMask : public Guide
{
public:
    GMask(std::shared_ptr<QImage> currFrame);
    virtual ~GMask();

protected:
    std::shared_ptr<QImage> getGuide();
    void setMask(std::shared_ptr<QImage> mask); //for user-provided masks

private:
    std::shared_ptr<QImage> m_mask;
    void createMask(std::shared_ptr<QImage> currFrame);
};

#endif // GMASK_H
