#ifndef STYLIZER_H
#define STYLIZER_H

#include "QImage"
//#include "guide.h"
#include "gedge.h"
#include "gpos.h"
#include "gmask.h"
#include "gtemp.h"
#include "iohandler.h"
#include <memory>

class Stylizer {
public:
    Stylizer(std::vector<std::shared_ptr<QImage>> inputFrames, std::vector<std::shared_ptr<QImage>> keyFrames, IOHandler &io);
    virtual ~Stylizer();
   std::pair<std::vector<QString>, std::vector<QString>> generateGuides(std::shared_ptr<QImage> key, int keyIdx, int beg, int end, int step);
    void run();
private:
//    void generateGuides(std::shared_ptr<QImage> keyFrame, std::shared_ptr<QImage> prevFrame, std::shared_ptr<QImage> currFrame);
    std::vector<std::shared_ptr<QImage>> m_frames;
    std::vector<std::shared_ptr<QImage>> m_keys;
    std::vector<std::shared_ptr<QImage>> m_output;

    IOHandler m_io;

//    std::shared_ptr<QImage> g_edge;
//    std::shared_ptr<QImage> g_mask;
//    std::shared_ptr<QImage> g_pos;
    //    std::shared_ptr<QImage> g_temp;
    std::vector<float> loadError(QString &binary);
};


#endif // STYLIZER_H
