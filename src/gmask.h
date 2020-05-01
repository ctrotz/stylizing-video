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
    void updateFrame(std::shared_ptr<QImage> frame, int i);

protected:
    QString getGuide();
    void setMask(std::shared_ptr<QImage> mask); //for user-provided masks

private:
//    std::shared_ptr<QImage> m_mask;
    QString m_mask;
    void createMask(std::shared_ptr<QImage> currFrame, int i);
};

#endif // GMASK_H
