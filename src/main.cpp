#include <QCoreApplication>
#include <QCommandLineParser>
#include <QImage>

#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <Eigen/Core>
#include "stylizer.h"

#include "iohandler.h"

//#include "gedge.h"
//#include "optical-flow/simpleflow.h"
//#include "opencvutils.h"
//#include "advector.h"
//#include "gpos.h"

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

    QCommandLineOption binary("binary", "Specifies alternate EbSynth location.", "location", "deps/ebsynth/bin/ebsynth");
    parser.addOption(binary);
    parser.process(a);
//    std::system("cd deps/ebsynth");

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
   QString binaryLoc = parser.value(binary);


    // Check arguments' validity
    if (begFrame > endFrame) {
        cerr << "Error: Beginning frame comes after end frame." << endl;
        a.exit(1);
        return 1;
    }

    IOHandler ioHandler(begFrame, endFrame, inputDir.toStdString(), keyframeDir.toStdString(), outputDir.toStdString(), binaryLoc.toStdString());

    vector<std::shared_ptr<QImage>> inputFrames;
    vector<std::shared_ptr<QImage>> keyframes;

    vector<std::shared_ptr<QImage>> advectedFrames;

    ioHandler.loadInputData(inputFrames, keyframes);

    GMask g_mask = GMask(inputFrames[10]);

//    std::shared_ptr<QImage> currFrame(new QImage("./data/minitest/video/000.jpg"));

//    GEdge guide(currFrame);

    Stylizer style(inputFrames, keyframes, ioHandler);
//    style.generateGuides();
    style.run();
    a.exit();


}


