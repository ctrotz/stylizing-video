#ifndef GPOS_H
#define GPOS_H

#include "guide.h"
#include <QObject>
#include <QImage>

#include <memory>

class GPos : public Guide
{
public:
    GPos(std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame, std::shared_ptr<QImage> g_mask);
    virtual ~GPos();
    static std::shared_ptr<QImage> generateGradient(int width, int height, QColor xColor = {255,0,0}, QColor yColor = {0,255,0});

protected:
    std::shared_ptr<QImage> getGuide();
    std::shared_ptr<QImage> getMotion(); //for G_temp

private:
    std::shared_ptr<QImage> m_guide;
    std::shared_ptr<QImage> m_motion;

    void createPos();
};

#endif // GPOS_H
