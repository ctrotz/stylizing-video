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
    getImageFilepaths();
}

void IOHandler::getImageFilepaths()
{
    // Get filepaths for input frames
    QDirIterator inputIt(_inputFramesDir);
    while (inputIt.hasNext()) {
        QString curPath = inputIt.next();
		QString filename = curPath.section('/', -1);

		if (isdigit(filename.toStdString()[0]) && (curPath.endsWith(".jpg") || curPath.endsWith(".jpeg"))) {
			int frameNum = std::stoi(filename.toStdString());

			// Check if within frame number range
			if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
				_inputFramePaths.push_back(curPath);
			}
		}
	}

    // Get filepaths for keyframes
    QDirIterator keyframesIt(_keyframesDir);
    while (keyframesIt.hasNext()) {
        QString curPath = keyframesIt.next();
		QString filename = curPath.section('/', -1);

		if (isdigit(filename.toStdString()[0]) && (curPath.endsWith(".jpg") || curPath.endsWith(".jpeg"))) {
			int frameNum = std::stoi(filename.toStdString());

			// Check if within frame number range
			if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
				_keyframePaths.push_back(curPath);
			}
		}
	}

	auto sorter = 
         [](const QString& s1, const QString& s2) {
		    std::string stdS1 = s1.toStdString();
		    std::string stdS2 = s2.toStdString();
            return std::lexicographical_compare(stdS1.begin(), stdS1.end(), stdS2.begin(), stdS2.end());
         };

    // Sort frames based on frame number
    sort(_inputFramePaths.begin(), _inputFramePaths.end(), sorter);

    // Sort keyframes based on frame number
    sort(_keyframePaths.begin(), _keyframePaths.end(), sorter);
}

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

void IOHandler::exportFrames(std::vector<QImage>& images)
{
	// Default: use _outputDir
	exportFrames(images, _outputDir);
}

void IOHandler::exportFrames(std::vector<QImage>& images, QDir outputDir)
{
	// Make output directory if it doesn't exist
	if (!outputDir.exists()) {
		outputDir.mkpath(".");
	}

	int x = images.size();  
    int padSize =
		(x < 100 ? 2 :
        (x < 1000 ? 3 :
        (x < 10000 ? 4 : 5)));

	// Export all images
	for (uint i = 0; i < images.size(); ++i) {
		QString folderPath = outputDir.path();
		QString filename = QString::number(i).rightJustified(padSize, '0');
		filename.append(".jpg");
		folderPath = folderPath.append("/").append(filename);
		images.at(i).save(folderPath, "JPG");
	}
}
