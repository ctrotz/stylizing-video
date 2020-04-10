#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <Eigen/Dense>
#include <QImage>

class IOHandler
{
public:
	IOHandler(int begFrame, int endFrame, std::string inputFramesDir,
			  std::string keyframesDir, std::string outputDir);

	void loadInputData(std::vector<QImage>& inputFrames, std::vector<QImage>& keyframes);
	void exportFrames(std::vector<QImage>& images);

private:
	void getImageFilepaths();

	int _begFrame;
	int _endFrame;
	std::string _inputFramesDir;
	std::string _keyframesDir;
	std::string _outputDir;
	std::vector<std::string> _inputFramePaths;
	std::vector<std::string> _keyframePaths;
};

#endif // IOHANDLER_H
