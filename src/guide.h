#ifndef GUIDE_H
#define GUIDE_H

#include <Eigen/Dense>
#include <QObject>

class Guide
{
public:
    Guide();
    Guide(std::shared_ptr<QImage> currFrame);

    virtual std::shared_ptr<QImage> getGuide() = 0;
    virtual ~Guide();
    virtual std::string getType() = 0;
};

#endif // GUIDE_H
