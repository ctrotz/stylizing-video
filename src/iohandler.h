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

    void loadInputData(std::vector<std::shared_ptr<QImage>>& inputFrames,
			           std::vector<std::shared_ptr<QImage>>& keyframes);

	void exportImages(const std::vector<std::shared_ptr<QImage>>& images,
		              const QDir outputDir);
	void exportImages(const std::vector<std::shared_ptr<QImage>>& images, 
					  const QDir outputDir,
					  const std::vector<QString>& filenames);

    void exportAllFrames(const std::vector<std::shared_ptr<QImage>>& images);
    void exportAllFrames(const std::vector<std::shared_ptr<QImage>>& images, const QDir outputDir);

	int getInputFrameNum(int frameIdx);
	int getKeyframeNum(int keyframeIdx);
	std::vector<int> getInputFrameNums();
	std::vector<int> getKeyframeNums();

private:
    void collectImageFilepaths();
	int calcNumDigits(int num);

    int _begFrame;
    int _endFrame;
    QDir _inputFramesDir;
    QDir _keyframesDir;
    QDir _outputDir;
    std::vector<QString> _inputFramePaths;
    std::vector<QString> _keyframePaths;
	std::vector<int> _inputFrameNums;
	std::vector<int> _keyframeNums;
};

#endif // IOHANDLER_H
