#ifndef STYLIZER_H
#define STYLIZER_H

#include "QImage"
//#include "guide.h"
#include "gedge.h"
#include "gpos.h"
#include "gmask.h"
#include "gtemp.h"
#include <memory>

class Stylizer {
public:
    Stylizer(std::vector<std::shared_ptr<QImage>> inputFrames, std::vector<std::shared_ptr<QImage>> keyFrames);
    virtual ~Stylizer();
    void generateGuides();
private:
//    void generateGuides(std::shared_ptr<QImage> keyFrame, std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame);
    std::vector<std::shared_ptr<QImage>> m_frames;
    std::vector<std::shared_ptr<QImage>> m_keys;
    std::vector<std::shared_ptr<QImage>> m_output;

//    std::shared_ptr<QImage> g_edge;
//    std::shared_ptr<QImage> g_mask;
//    std::shared_ptr<QImage> g_pos;
//    std::shared_ptr<QImage> g_temp;
};


#endif // STYLIZER_H
