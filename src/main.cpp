#include <QCoreApplication>
#include <QCommandLineParser>
#include <QImage>

#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <Eigen/Core>

#include "iohandler.h"
#include "optical-flow/simpleflow.h"
#include "opencvutils.h"
#include "advector.h"
#include "gpos.h"
#include "fft_fsolver.h"

using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("inputDir", "File path to directory with input frames");
    parser.addPositionalArgument("outputDir", "File path to directory for output frames");
    parser.addPositionalArgument("keyframeDir", "File path to directory with keyframes");
    parser.addPositionalArgument("begFrame", "Optional first frame in sequence");
    parser.addPositionalArgument("endFrame", "Optional last frame in sequence");

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if((args.size() != 3) && (args.size() != 5)) {
        cerr << "Error: Wrong number of arguments" << endl;
        parser.showHelp();
        a.exit(1);
        return 1;
    }

    QString inputDir = args[0];
    QString outputDir = args[1];
    QString keyframeDir = args[2];
    int begFrame = -1;
    int endFrame = -1;

    if (args.size() == 5){
        begFrame = args[3].toInt();
        endFrame = args[4].toInt();
    }

    // Check arguments' validity
    if (begFrame > endFrame) {
        cerr << "Error: Beginning frame comes after end frame." << endl;
        a.exit(1);
        return 1;
    }

    IOHandler ioHandler(begFrame, endFrame, inputDir, keyframeDir, outputDir);

    vector<std::shared_ptr<QImage>> inputFrames;
    vector<std::shared_ptr<QImage>> keyframes;

    vector<std::shared_ptr<QImage>> advectedFrames;

    ioHandler.loadInputData(inputFrames, keyframes);

    Advector advector = Advector();
    std::shared_ptr<QImage> mask(new QImage(*inputFrames.at(0)));

    mask->fill(Qt::white);
    GPos gpos_start = GPos(mask);
    GPos gpos_cur = gpos_start;
    advectedFrames.push_back(gpos_cur.getGuide());
    Mat i1, i2;
    Mat2f out;
    Mat3b b1, b2;
    Mat3f gx, gy;
    vector<Mat> rgbChannels(3);
    vector<Mat> gradX(3);
    vector<Mat> gradY(3);

    for (uint i = 0; i < inputFrames.size() - 1; i++) {
        i1 = qimage_to_mat_ref((*inputFrames.at(i)));
        i2 = qimage_to_mat_ref((*inputFrames.at(i + 1)));

        cvtColor(i1, i1, COLOR_BGRA2BGR);
        cvtColor(i2, i2, COLOR_BGRA2BGR);

        b1 = static_cast<Mat3b>(i1);
        b2 = static_cast<Mat3b>(i2);

        cv::split(b1, rgbChannels);
        for (int i = 0; i < 3; i++) {
            cv::Sobel(rgbChannels.at(i), gradX.at(i), CV_32F, 1, 0);
            cv::Sobel(rgbChannels.at(i), gradY.at(i), CV_32F, 0, 1);
        }

        cv::merge(gradX, gx);
        cv::merge(gradY, gy);
        cv::imshow("gx_b", gradX.at(0));
        cv::imshow("gy_b", gradY.at(0));
        cv::imshow("b1 pre", b1);
        cv::imshow("b2 pre", b2);
        cv::waitKey(0);


        fourierSolve(b1, gx, gy, 1);
        cv::imshow("solved", b1);
        cv::waitKey(0);

//        std::cout << "flow #" << to_string(i) <<  " calculated" << std::endl;
//        out = calculateFlow(i1, i2, false, false);
//        gpos_cur.advect(mask, out);
//        advectedFrames.push_back(gpos_cur.getGuide());
    }
    ioHandler.exportImages(advectedFrames, QDir("./gpos_inpaint"));

    a.exit();


}


