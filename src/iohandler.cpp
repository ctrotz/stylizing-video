#include "iohandler.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;


IOHandler::IOHandler(int begFrame, int endFrame, std::string inputFramesDir,
					 std::string keyframesDir, std::string outputDir) :
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
	for (const auto& entry : fs::directory_iterator(_inputFramesDir)) {
		fs::path curPath = entry.path();

		if (curPath.extension() == ".jpg" && isdigit(curPath.stem().c_str()[0])) {
			int frameNum = std::stoi(curPath.stem().c_str());

			// Check if within frame number range
			if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
				_inputFramePaths.push_back(entry.path().c_str());
			}
		}
	}

	// Get filepaths for keyframes
	for (const auto& entry : fs::directory_iterator(_keyframesDir)) {
		fs::path curPath = entry.path();

		if (curPath.extension() == ".jpg" && isdigit(curPath.stem().c_str()[0])) {
			int frameNum = std::stoi(curPath.stem().c_str());

			// Check if within frame number range
			if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
				_keyframePaths.push_back(entry.path().c_str());
			}
		}
	}

	// Sort frames based on frame number
	sort(_inputFramePaths.begin(), _inputFramePaths.end(),
         [](const std::string& s1, const std::string& s2) {
         	return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end());
         });

	// Sort keyframes based on frame number
	sort(_keyframePaths.begin(), _keyframePaths.end(),
	     [](const std::string& s1, const std::string& s2) {
	     	return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end());
	     });
}

void IOHandler::loadInputData(std::vector<QImage>& inputFrames, std::vector<QImage>& keyframes)
{
	// Import input frames
	for (std::string inputFramePath : _inputFramePaths) {
		inputFrames.push_back(QImage(inputFramePath.c_str()));
	}

	// Import keyframes
	for (std::string keyframePath : _keyframePaths) {
		keyframes.push_back(QImage(keyframePath.c_str()));
	}
}

void IOHandler::exportFrames(std::vector<QImage>& images)
{
	if (!fs::exists(_outputDir)) {
		fs::create_directory(_outputDir);
	}

	for (uint i = 0; i < images.size(); ++i) {
		fs::path curPath = fs::path(_outputDir).append(std::to_string(i).append(".jpg"));
		images.at(i).save(curPath.c_str(), "JPG");
	}
}
