#ifndef STYLIZER_H
#define STYLIZER_H

#include "QImage"
#include <memory>

class Stylizer {
public:
    Stylizer();
private:
    void generateGuides(std::shared_ptr<QImage> keyFrame, std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame);
};


#endif // STYLIZER_H
