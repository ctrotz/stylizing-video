#include "stylizer.h"
#include "opencvutils.h"
#include "optical-flow/simpleflow.h"
#include "serialize.h"
#define GENERATE false

using namespace std;

Stylizer::Stylizer(std::vector<shared_ptr<QImage>> inputFrames, std::vector<shared_ptr<QImage>> keyFrames, IOHandler &io) :
    m_frames(inputFrames), m_keys(keyFrames), m_io(io)
{
    m_advects.reserve(m_frames.size());
}

Stylizer::~Stylizer(){
    m_frames.clear();
    m_keys.clear();
    m_advects.clear();
}

void Stylizer::run(){
    for (int i = 0; i < m_keys.size()-1; i++){
        int beg = m_io.getKeyframeNum(i);
        int end = m_io.getKeyframeNum(i+1);
        // treat beg/end as indices for input frames
        std::pair<std::vector<QString>, std::vector<QString>> a;
        std::pair<std::vector<QString>, std::vector<QString>> b;
        if (GENERATE){
            a = generateGuides(m_keys.at(i), i, beg, end, 1);
            b = generateGuides(m_keys.at(i+1), i+1, end, beg, -1);
        } else {
            a = fetchGuides(i, beg, end, 1);
            b = fetchGuides(i+1, end, beg, -1);
        }
        std::reverse(b.first.begin(), b.first.end());
        std::reverse(b.second.begin(), b.second.end());

        //blend a and b-- NOTE: a and b will be missing the keyframes-- just substitute in keyframes instead of blending
        std::vector<cv::Mat> masks = createMasks(a, b);
//        std::vector<cv::Mat> final_masks = tempCoherence(masks);

//        namedWindow( "Display window", WINDOW_AUTOSIZE );
//        imshow( "Display window", final_masks[50]*255);
//        waitKey(0);
    }
}

std::vector<cv::Mat> Stylizer::tempCoherence(std::vector<cv::Mat> masks){
    std::vector<cv::Mat> final_masks;
    final_masks.reserve(masks.size());
    int width = masks[0].cols;
    int height = masks[0].rows;
    cv::Size size(width, height);
    Advector advector = Advector();

    for (int i=1; i < masks.size(); i++){
        Mat prevMask = masks[i-1];
        Mat currMask = masks[i];
        Mat2f flowField = deserializeMatbin(m_flowpaths[i]);
//        std::cout << flowField.at<cv::Vec2f>(10,10)<< std::endl;
        Mat advected = Mat::zeros(size, CV_8UC1);
//        std::cout << advected.rows << " " << advected.cols << std::endl;
        advector.advectMask(flowField, prevMask, advected);
//        std::cout << advected.rows << " " << advected.cols << std::endl;
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
//                std::cout << "advected: " << advected.at<float>(r,c) << std::endl;
//                std::cout << "mask: " << currMask.at<float>(r,c) << std::endl;
                if (advected.at<float>(r,c)< 0.1f && currMask.at<float>(r,c)> 0.9f){
                    advected.at<float>(r,c)=1.f;
                }
            }
        }
        final_masks.push_back(advected);
    }
    return final_masks;
}

std::pair<std::vector<QString>, std::vector<QString>> Stylizer::fetchGuides(int keyIdx, int beg, int end, int step) {
    std::vector<QString> outpaths;
    outpaths.reserve(abs(end-beg) + 1);
    std::vector<QString> errorpaths;
    errorpaths.reserve(abs(end-beg) + 1);
//    Mat i1, i2;
    for (int i = beg+step; i != end; i+=step){
        if (step > 0){
//            i1 = qimage_to_mat_ref((*m_frames.at(i-step)));
//            i2 = qimage_to_mat_ref((*m_frames.at(i)));

//            cvtColor(i1, i1, COLOR_BGRA2BGR);
//            cvtColor(i2, i2, COLOR_BGRA2BGR);

//            Mat2f out = calculateFlow(i1, i2, false, false);
            std::string filename = "flowfields/" + std::to_string(i) + ".matbin";
//            serializeMatbin(out, filename);
//            m_advects.push_back(out);
            m_flowpaths.push_back(filename);
        }
        QString frame = QString::number(i).rightJustified(3, '0');
        QString prefix("outtest/" + QString::number(keyIdx) + "-" + frame );
        QString outfile(prefix+".png");
        outpaths.push_back(outfile);
        errorpaths.push_back(prefix + ".bin");
    }
    return {outpaths,errorpaths};
}

std::vector<cv::Mat> Stylizer::createMasks(std::pair<std::vector<QString>, std::vector<QString>> a, std::pair<std::vector<QString>, std::vector<QString>> b){
    std::vector<cv::Mat> masks;
    int width = m_frames[0]->width();
    int height = m_frames[0]->height();
    cv::Size size(width, height);
    masks.reserve(a.first.size()+2);
    masks.push_back(Mat::zeros(size, CV_8UC1));
    for (int frame=0; frame < a.first.size(); frame++){
        std::vector<float> error_a = loadError(a.second[frame]);
        std::vector<float> error_b = loadError(b.second[frame]);
        Mat mask(size, CV_8UC1);
        uint8_t *maskData = mask.data;
        for (int i = 0; i < error_a.size(); i++){
            maskData[i] = error_a[i] < error_b[i] ? 0 : 1;
        }
        mask.convertTo(mask, CV_8UC1);
        masks.push_back(mask);
        namedWindow( "Display window", WINDOW_AUTOSIZE );
        imshow( "Display window", mask*255);
        waitKey(0);
    }
    masks.push_back(Mat::ones(size, CV_8UC1));
    return masks;
}

std::vector<float> Stylizer::loadError(QString& binary) {
    std::vector<float> out;
    std::ifstream in =  std::ifstream(binary.toStdString(), std::ifstream::binary);
    deserialize(in, out);
    return out;
}

std::pair<std::vector<QString>, std::vector<QString>> Stylizer::generateGuides(shared_ptr<QImage> keyframe, int keyIdx, int beg, int end, int step) {
    std::vector<QString> outpaths;
    outpaths.reserve(abs(end-beg) + 1);
    std::vector<QString> errorpaths;
    errorpaths.reserve(abs(end-beg) + 1);

    QString prevEdge;

    std::shared_ptr<QImage> key(new QImage(*keyframe));
    std::shared_ptr<QImage> mask(new QImage(*m_frames.at(beg)));

    std::shared_ptr<QImage> frame1(new QImage(*m_frames.at(beg)));

    GEdge edge(frame1);
    prevEdge = edge.getGuide(beg);

    mask->fill(Qt::white);
    GPos gpos_start = GPos(mask);
    GPos gpos_cur = gpos_start;

    QString prevPos = gpos_cur.getGuide(beg);

    Mat i1, i2;
//    Mat2f out;

    int keyNum =  m_io.getKeyframeNum(keyIdx);
    QString keynum = QString::number(keyNum).rightJustified(3, '0');
    QString prevTemp("./data/test/keys/" + keynum + ".jpg");
    std::shared_ptr<QImage> prevStylizedFrame(new QImage(*key));
    GTemp gtemp;


    for (int i = beg+step; i != end; i+=step){
        std::shared_ptr<QImage> frame2(new QImage(*m_frames.at(i)));

        edge.updateFrame(frame2);
        QString g_edge2 = edge.getGuide(i);

        i1 = qimage_to_mat_ref((*m_frames.at(i-step)));
        i2 = qimage_to_mat_ref((*m_frames.at(i)));

        cvtColor(i1, i1, COLOR_BGRA2BGR);
        cvtColor(i2, i2, COLOR_BGRA2BGR);

        Mat2f out = calculateFlow(i1, i2, false, false);
        if (step > 0){
            m_advects.push_back(out);
        }
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

        QString prefix("outtest/" + QString::number(keyIdx) + "-" + frame );

        QString outfile(prefix+".png");
        command.append("-output ../../" + outfile);

        command.append(" -searchvoteiters 12 -patchmatchiters 6");

        QByteArray ba = command.toLocal8Bit();
        const char *c_str = ba.data();
        std::system(c_str);
//        std::cout << c_str << std::endl;
        prevStylizedFrame = std::make_shared<QImage>(outfile);

        outpaths.push_back(outfile);
        errorpaths.push_back(prefix + ".bin");
    }
    return {outpaths,errorpaths};
}
