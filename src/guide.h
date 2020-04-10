#ifndef GUIDE_H
#define GUIDE_H

#include <Eigen/Dense>

class Guide
{
public:
    Guide();

    virtual void apply() = 0;
};

#endif // GUIDE_H
