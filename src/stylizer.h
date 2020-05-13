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
    void generateGuides(std::shared_ptr<QImage> key, Sequence& s);
    std::pair<std::vector<QString>, std::vector<QString>> fetchGuides(int keyIdx, int beg, int end, int step);
    void run();
    std::vector<cv::Mat> createMasks(std::pair<std::vector<QString>, std::vector<QString>> a, std::pair<std::vector<QString>, std::vector<QString>> b);
private:
    std::vector<std::shared_ptr<QImage>> m_frames;
    std::vector<std::shared_ptr<QImage>> m_keys;
    std::vector<std::shared_ptr<QImage>> m_output;
    std::vector<cv::Mat2f> m_advects;
    std::vector<std::string> m_flowpaths;
    std::vector<Sequence> m_seqs;

    IOHandler m_io;

    std::vector<float> loadError(QString &binary);
    std::vector<cv::Mat> tempCoherence(std::vector<cv::Mat> masks);

    void poissonBlend(std::vector<cv::Mat> &hp_blends, const std::vector<cv::Mat3f> &gradX, const std::vector<cv::Mat3f> &gradY, std::vector<cv::Mat> &out_frames); 

};


#endif // STYLIZER_H
