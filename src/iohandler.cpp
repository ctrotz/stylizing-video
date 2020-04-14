#include "iohandler.h"

#include <iostream>

#include <QDirIterator>

IOHandler::IOHandler(int begFrame, int endFrame, QString inputFramesDir,
                     QString keyframesDir, QString outputDir) :
    _begFrame(begFrame),
    _endFrame(endFrame),
    _inputFramesDir(inputFramesDir),
    _keyframesDir(keyframesDir),
    _outputDir(outputDir)
{
    collectImageFilepaths();
}

void IOHandler::collectImageFilepaths()
{
    // Get filepaths for input frames
    QDirIterator inputIt(_inputFramesDir);
    while (inputIt.hasNext()) {
        QString curPath = inputIt.next();
        QString filename = curPath.section('/', -1);

        if (isdigit(filename.toStdString()[0]) &&
            (curPath.endsWith(".jpg") || curPath.endsWith(".jpeg"))) {
            int frameNum = std::stoi(filename.toStdString());

            // Check if within frame number range
            if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
                _inputFramePaths.push_back(curPath);
                _inputFrameNums.push_back(frameNum);
            }
        }
    }

    // Get filepaths for keyframes
    QDirIterator keyframesIt(_keyframesDir);
    while (keyframesIt.hasNext()) {
        QString curPath = keyframesIt.next();
        QString filename = curPath.section('/', -1);

        if (isdigit(filename.toStdString()[0]) &&
            (curPath.endsWith(".jpg") || curPath.endsWith(".jpeg"))) {
            int frameNum = std::stoi(filename.toStdString());

            // Check if within frame number range
            if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
                _keyframePaths.push_back(curPath);
                _keyframeNums.push_back(frameNum);
            }
        }
    }

    auto sorter = 
         [](const QString& s1, const QString& s2) {
            std::string stdS1 = s1.toStdString();
            std::string stdS2 = s2.toStdString();
            return std::lexicographical_compare(stdS1.begin(), stdS1.end(),
                                                stdS2.begin(), stdS2.end());
         };

    // Sort frames based on frame number
    sort(_inputFramePaths.begin(), _inputFramePaths.end(), sorter);

    // Sort keyframes based on frame number
    sort(_keyframePaths.begin(), _keyframePaths.end(), sorter);

	// Sort the frame andn keyframe nums
	sort(_inputFrameNums.begin(), _inputFrameNums.end());
	sort(_keyframeNums.begin(), _keyframeNums.end());
}

// Loads input frames and keyframes into the provided vectors.
void IOHandler::loadInputData(std::vector<QImage>& inputFrames, std::vector<QImage>& keyframes)
{
    // Import input frames
    for (QString inputFramePath : _inputFramePaths) {
        inputFrames.push_back(QImage(inputFramePath));
    }

    // Import keyframes
    for (QString keyframePath : _keyframePaths) {
        keyframes.push_back(QImage(keyframePath));
    }
}

// Defaults to using the index of the image in "images" as the
// the filename of the exported image.
void IOHandler::exportImages(const std::vector<QImage>& images,
                             const QDir outputDir)
{
    int padSize = calcNumDigits(images.size());

    std::vector<QString> filenames;

    for (uint i = 0; i < images.size(); ++i) {
        QString outPath = outputDir.path();
        QString filename = QString::number(i).rightJustified(padSize, '0');
        filename.append(".jpg");

        filenames.push_back(filename);
    }

    exportImages(images, outputDir, filenames);
}

// General exporting method for images
void IOHandler::exportImages(const std::vector<QImage>& images, 
                             const QDir outputDir,
                             const std::vector<QString>& filenames)
{
    if (images.size() != filenames.size()) {
        std::cerr << "Error: Number of images to export does not equal "
                  << "number of filenames provided." << std::endl;
        return;
    }

    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    // Export all images
    for (uint i = 0; i < images.size(); ++i) {
        QString outPath = outputDir.path();
        QString filename = filenames.at(i);
        filename.append(".jpg");
        outPath = outPath.append("/").append(filename);

        images.at(i).save(outPath, "JPG");
    }
}


// Defaults to using _outputDir as the output directory.
void IOHandler::exportAllFrames(const std::vector<QImage>& images)
{
    exportAllFrames(images, _outputDir);
}

// This function is meant for exporting the same number of images as input frames,
// using the names/numbers of these frames as the filenames of the exported images.
void IOHandler::exportAllFrames(const std::vector<QImage>& images, const QDir outputDir)
{
    if (images.size() != _inputFrameNums.size()) {
        std::cerr << "Error: Number of images to export does "
                  << "not equal number of input frames." << std::endl;
        return;
    }

    // Make output directory if it doesn't exist
    if (!outputDir.exists()) {
        outputDir.mkpath(".");
    }

    int padSize = calcNumDigits(images.size());

    std::vector<QString> filenames;

    // Export all images
    for (uint i = 0; i < images.size(); ++i) {
        QString outPath = outputDir.path();
        QString filename = QString::number(_inputFrameNums.at(i)).rightJustified(padSize, '0');
        filename.append(".jpg");

        filenames.push_back(filename);
    }

    exportImages(images, outputDir, filenames);
}

// Get the frame number associated to the keyframe at index keyframeIdx
int IOHandler::getKeyframeNum(int keyframeIdx)
{
    return _keyframeNums.at(keyframeIdx);
}

// Get vector of all frame numbers associated to the keyframes
std::vector<int> IOHandler::getKeyframeNums()
{
    return _keyframeNums;
}

// Calculates the number of digits needed to represent a number in base 10.
// Useful for the naming of exported image files.
int IOHandler::calcNumDigits(int num)
{
    return (num < 100 ? 2 :
           (num < 1000 ? 3 :
           (num < 10000 ? 4 :
           (num < 100000 ? 5 :
           (num < 1000000 ? 6 :
           (num < 10000000 ? 7 :
           (num < 100000000 ? 8 :
           (num < 1000000000 ? 9 : 10))))))));
}

