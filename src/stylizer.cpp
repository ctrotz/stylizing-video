#include "stylizer.h"
#include "opencvutils.h"
#include "optical-flow/simpleflow.h"

using namespace std;

Stylizer::Stylizer(std::vector<shared_ptr<QImage>> inputFrames, std::vector<shared_ptr<QImage>> keyFrames, IOHandler &io) :
    m_frames(inputFrames), m_keys(keyFrames), m_io(io)
{
}

Stylizer::~Stylizer(){
    m_frames.clear();
    m_keys.clear();
}

void Stylizer::run(){
    for (int i = 0; i < m_keys.size()-1; i++){
        int beg = m_io.getKeyframeNum(i);
        int end = m_io.getKeyframeNum(i+1);
        // treat beg/end as indices for input frames
        std::vector<QString> a;
        std::vector<QString> b;
        a = generateGuides(m_keys.at(i), i, beg, end, 1);
        b = generateGuides(m_keys.at(i+1), i+1, end, beg, -1);
        //blend a and b-- NOTE: a and b will be missing the keyframes-- just substitute in keyframes instead of blending
    }
}

std::vector<QString> Stylizer::generateGuides(shared_ptr<QImage> keyframe, int keyIdx, int beg, int end, int step) {
    std::vector<QString> outpaths;
    outpaths.reserve(abs(end-beg) + 1);
    QString prevEdge;

    std::shared_ptr<QImage> key(new QImage(*keyframe));
    std::shared_ptr<QImage> mask(new QImage(*m_frames.at(beg)));

    std::shared_ptr<QImage> frame1(new QImage(*m_frames.at(beg)));

    GEdge edge(frame1);
    edge.updateFrame(frame1, beg);
    prevEdge = edge.getGuide();

    mask->fill(Qt::white);
    GPos gpos_start = GPos(mask);
    GPos gpos_cur = gpos_start;

    QString prevPos = gpos_cur.getGuide(beg);

    Mat i1, i2;
    Mat2f out;

    int keyNum =  m_io.getKeyframeNum(keyIdx);
    QString keynum = QString::number(keyNum).rightJustified(3, '0');
    QString prevTemp("./data/test/keys/" + keynum + ".jpg");
    std::shared_ptr<QImage> prevStylizedFrame(new QImage(*key));
    GTemp gtemp;


    for (int i = beg+step; i != end; i+=step){
        std::shared_ptr<QImage> frame2(new QImage(*m_frames.at(i)));

        edge.updateFrame(frame2, i);
        QString g_edge2 = edge.getGuide();

//        std::shared_ptr<QImage> frame1m(new QImage(m_frames(i)->copy()));
//        std::shared_ptr<QImage> frame2m(new QImage(m_frames(i+1)->copy()));
//        GMask mask(frame1m);
//        QString g_mask1 = mask->getGuide();
//        mask->updateFrame(frame2m);
//        QString g_mask2 = mask->getGuide();

        i1 = qimage_to_mat_ref((*m_frames.at(i-step)));
        i2 = qimage_to_mat_ref((*m_frames.at(i)));

        cvtColor(i1, i1, COLOR_BGRA2BGR);
        cvtColor(i2, i2, COLOR_BGRA2BGR);

        out = calculateFlow(i1, i2, false, false);
        gpos_cur.advect(mask, out);
        QString g_pos2 = gpos_cur.getGuide(i);

        gtemp.updateGuide(prevStylizedFrame, out, mask);
        QString g_temp2 = gtemp.getGuide(i);

        QString prevframe = QString::number(beg).rightJustified(3, '0');
        QString frame = QString::number(i).rightJustified(3, '0');

        QString command("cd ./deps/ebsynth && bin/ebsynth -style ../../data/test/keys/" + keynum + ".jpg ");
        command.append("-guide ../." + prevEdge + " ../." + g_edge2 + " -weight 0.5 ");

//        command.append("-guide " + g_mask1 + " " + g_mask2 + " -weight 6 ");

        command.append("-guide ../../data/test/video/" + prevframe + ".jpg " + "../../data/test/video/" + frame + ".jpg -weight 6 ");

        command.append("-guide ../." + prevPos + " ../." + g_pos2 + " -weight 2 ");

        command.append("-guide ../." + prevTemp + " ../." + g_temp2 + " -weight 0.5 ");

        QString outfile("outtest/" + QString::number(keyIdx) + "-" + frame + ".png");

        command.append("-output ../../" + outfile);

        command.append(" -searchvoteiters 12 -patchmatchiters 6");

        QByteArray ba = command.toLocal8Bit();
        const char *c_str = ba.data();
        std::system(c_str);
        prevStylizedFrame = std::make_shared<QImage>(outfile);
        outpaths.push_back(outfile);
    }
    return outpaths;
}
