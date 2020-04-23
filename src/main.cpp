#include <QCoreApplication>
#include <QCommandLineParser>
#include <QImage>

#include <unistd.h>
#include <iostream>
#include <Eigen/Core>
#include "gmask.h"

#include "iohandler.h"

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

    vector<QImage> inputFrames;
    vector<QImage> keyframes;

    ioHandler.loadInputData(inputFrames, keyframes);

    GMask mask = GMask(make_shared<QImage>(inputFrames[0]));
    mask.m_mask->save("./temp.jpg");
    GMask mask2 = GMask(make_shared<QImage>(inputFrames[1]));
    mask2.m_mask->save("./temp1.jpg");

    ioHandler.exportAllFrames(inputFrames);

    a.exit();
}
