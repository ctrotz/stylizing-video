#ifndef GUIDE_H
#define GUIDE_H

#include <Eigen/Dense>
#include <QObject>

class Guide
{
public:
    Guide();
    Guide(std::shared_ptr<QImage> currFrame);

    virtual ~Guide();

protected:
    virtual std::shared_ptr<QImage> getGuide() = 0;
};

#endif // GUIDE_H
