#ifndef STYLIZER_H
#define STYLIZER_H

#include "QImage"

class Stylizer {
public:
    Stylizer();
private:
    void generateGuides(QImage keyFrame, QImage prevFrame, QImage currFrame);
};


#endif // STYLIZER_H
