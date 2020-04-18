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

    Mat i1, i2, out;
    for (uint i = 0; i < inputFrames.size() - 1; i++) {
        i1 = qimage_to_mat_ref((*inputFrames.at(i)), CV_8UC3);
        i2 = qimage_to_mat_ref((*inputFrames.at(i + 1)), CV_8UC3);
        std::cout << "flow #" << to_string(i) <<  " calculated" << std::endl;
        out = calculateFlow(i1, i2);

        std::shared_ptr<QImage> newFrame(new QImage(*inputFrames.at(i)));
        std::shared_ptr<QImage> mask(new QImage(*inputFrames.at(i)));

        mask->fill(Qt::white);

        if (i == 0) {
            advector.advect(out, mask, GPos::generateGradient((*inputFrames.at(i)).width(), (*inputFrames.at(i)).height()), newFrame);
        } else {
            advector.advect(out, mask, advectedFrames.at(i-1), newFrame);
        }
        advectedFrames.push_back(newFrame);

    }
    ioHandler.exportImages(advectedFrames, QDir("./gpos"));

    a.exit();


}


