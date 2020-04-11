#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <Eigen/Dense>
#include <QImage>
#include <QDir>

class IOHandler
{
public:
    IOHandler(int begFrame, int endFrame, QString inputFramesDir,
              QString keyframesDir, QString outputDir);

    void loadInputData(std::vector<QImage>& inputFrames, std::vector<QImage>& keyframes);
    void exportFrames(std::vector<QImage>& images);
    void exportFrames(std::vector<QImage>& images, QDir outputDir);

private:
    void getImageFilepaths();

    int _begFrame;
    int _endFrame;
    QDir _inputFramesDir;
    QDir _keyframesDir;
    QDir _outputDir;
    std::vector<QString> _inputFramePaths;
    std::vector<QString> _keyframePaths;
};

#endif // IOHANDLER_H
