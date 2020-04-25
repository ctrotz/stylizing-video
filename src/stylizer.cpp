#include "stylizer.h"
#include "opencvutils.h"
#include "optical-flow/simpleflow.h"

using namespace std;

Stylizer::Stylizer(std::vector<shared_ptr<QImage>> inputFrames, std::vector<shared_ptr<QImage>> keyFrames) :
    m_frames(inputFrames), m_keys(keyFrames)
{
//    m_output.reserve(inputFrames.size());
}

Stylizer::~Stylizer(){
    m_frames.clear();
    m_keys.clear();
//    m_output.clear();
}

void Stylizer::generateGuides() {
    QString prevEdge;

    std::shared_ptr<QImage> key(new QImage(*m_keys.at(0)));
    std::shared_ptr<QImage> mask(new QImage(*m_frames.at(0)));

    std::shared_ptr<QImage> frame1(new QImage(*m_frames.at(0)));
    GEdge edge(frame1);
    prevEdge = edge.getGuide();

    mask->fill(Qt::white);
    GPos gpos_start = GPos(mask);
    GPos gpos_cur = gpos_start;
//    QString prevPos;
//    QString g_pos2;
    QString prevPos = gpos_cur.getGuide(0);
    Mat i1, i2;
    Mat2f out;

    QString prevTemp("./data/test/keys/000.jpg");
    std::shared_ptr<QImage> prevStylizedFrame(new QImage(*m_keys.at(0)));
    GTemp gtemp;//(prevStylizedFrame, out, mask);

    for (int i = 1; i < m_frames.size(); i++){ //m_frames.size()
//        std::shared_ptr<QImage> frame1(new QImage(*m_frames.at(i)));
        std::shared_ptr<QImage> frame2(new QImage(*m_frames.at(i)));

//        GEdge edge(frame1);
//        QString g_edge1 = edge.getGuide();
        edge.updateFrame(frame2, i);
        QString g_edge2 = edge.getGuide();

//        std::shared_ptr<QImage> frame1m(new QImage(m_frames(i)->copy()));
//        std::shared_ptr<QImage> frame2m(new QImage(m_frames(i+1)->copy()));
//        GMask mask(frame1m);
//        QString g_mask1 = mask->getGuide();
//        mask->updateFrame(frame2m);
//        QString g_mask2 = mask->getGuide();

//        QString g_pos1 = gpos_cur.getGuide(i);

        i1 = qimage_to_mat_ref((*m_frames.at(i-1)));
        i2 = qimage_to_mat_ref((*m_frames.at(i)));

        cvtColor(i1, i1, COLOR_BGRA2BGR);
        cvtColor(i2, i2, COLOR_BGRA2BGR);

        out = calculateFlow(i1, i2, false, false);
        gpos_cur.advect(mask, out);
        QString g_pos2 = gpos_cur.getGuide(i);

//        GTemp gtemp(prevStylizedFrame, out, mask);
        gtemp.updateGuide(prevStylizedFrame, out, mask);
        QString g_temp2 = gtemp.getGuide(i);

        QString prevframe = QString::number(0).rightJustified(3, '0');
        QString frame = QString::number(i).rightJustified(3, '0');

        QString command("cd ./deps/ebsynth && bin/ebsynth -style ../../data/test/keys/000.jpg ");
        command.append("-guide ../.");
        command.append(prevEdge);
        command.append(" ../.");
        command.append(g_edge2);
        command.append(" -weight 0.5 ");

//        command.append("-guide ");
//        command.append(g_mask1);
//        command.append(" ");
//        command.append(g_mask2);
//        command.append(" -weight 6 ");

        command.append("-guide ../../data/test/video/");
        command.append(prevframe);
        command.append(".jpg ../../data/test/video/");
        command.append(frame);
        command.append(".jpg -weight 6 ");

        command.append("-guide ../.");
        command.append(prevPos);
        command.append(" ../.");
        command.append(g_pos2);
        command.append(" -weight 2 ");

        command.append("-guide ../.");
        command.append(prevTemp);
        command.append(" ../.");
        command.append(g_temp2);
        command.append(" -weight 0.5 ");

        command.append("-output ../../outtest/out");
        command.append(frame);
//        command.append(QString::number(i));
        command.append(".png");
        command.append(" -searchvoteiters 12 -patchmatchiters 6");

        QByteArray ba = command.toLocal8Bit();
        const char *c_str = ba.data();
        std::system(c_str);
        prevStylizedFrame = std::make_shared<QImage>("outtest/out" + frame + ".png");

    }
}
