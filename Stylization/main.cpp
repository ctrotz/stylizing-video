#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>

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
        a.exit(1);
        return 1;
    }

    QString inputDir = args[0];
    QString outputDir = args[1];
    QString keyframeDir = args[2];
    int begFrame;
    int endFrame;

    if (args.size() == 5){
        begFrame = args[3].toInt();
        endFrame = args[4].toInt();
    }

    //Load frames from inputDir.toStdString() + frame #
    //Load keyframes from keyframeDir.toStdString() + keyframe #


    //Save output frame to outputDir.toStdString() + frame #

    a.exit();
}
