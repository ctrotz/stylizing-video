#ifndef GUIDE_H
#define GUIDE_H

#include <Eigen/Dense>
#include <QObject>

class Guide
{
public:
    Guide();
    Guide(QImage *currFrame);

    virtual ~Guide();

protected:
    virtual QImage* getGuide() = 0;
};

#endif // GUIDE_H
