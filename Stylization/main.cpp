#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("prevFrame", "Previous input frame file path");
    parser.addPositionalArgument("currFrame", "Input frame file path");
    parser.addPositionalArgument("outfile", "Output frame file path");
    parser.addPositionalArgument("keyframe", "Input keyframe");

    parser.addPositionalArgument("args1", "respective argument for the method");
    parser.addPositionalArgument("args2", "respective argument2 for the method");
    parser.addPositionalArgument("args3", "respective argument3 for the method");
    parser.addPositionalArgument("args4", "respective argument3 for the method");

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if(args.size() != 4) {
        cerr << "Error: Wrong number of arguments" << endl;
        a.exit(1);
        return 1;
    }

    QString prevFrame = args[0];
    QString currFrame = args[1];
    QString outfile = args[2];
    QString keyframe = args[3];

    //Load frames from prevFrame.toStdString() and currFrame.toStdString()
    //Load keyframe from keyframe.toStdString()


    //Save output frame to outfile.toStdString()

    a.exit();
}
