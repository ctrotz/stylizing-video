#include "iohandler.h"

#include <iostream>
#include <regex>
#include <filesystem>

IOHandler::IOHandler(int begFrame, int endFrame, std::string inputFramesDir,
                     std::string keyframesDir, std::string outputDir, std::string binaryLocation) :
    _begFrame(begFrame),
    _endFrame(endFrame),
    _inputFramesDir(inputFramesDir),
    _keyframesDir(keyframesDir),
    _outputDir(outputDir),
    _binaryLocation(binaryLocation)
{
    if (!fs::exists(_inputFramesDir) || !fs::exists(_keyframesDir)) {
        throw std::invalid_argument("Non-existent input and/or keyframe directory and/or ebsynth binary.");
    } else if  (!fs::exists(_binaryLocation) || _binaryLocation.stem() != "ebsynth") {
        throw std::invalid_argument("Non-existent ebsynth binary.");
    }

    collectImageFilepaths();
}

void IOHandler::collectImageFilepaths()
{
    // Get filepaths for input frames
    std::regex numberFiles = std::regex("^[0-9]*\\.(jpg|JPG|jpeg|JPEG|png|PNG)$");
    fs::directory_iterator inputIt(_inputFramesDir);
    for (fs::directory_entry const& entry : inputIt) {
        if (!entry.is_regular_file()) {
            continue;
        }
        fs::path const& path = entry.path();
        if (std::regex_match(path.filename().string(), numberFiles)) {
            int frameNum = std::stoi(path.stem());

            // Check if within frame number range
            if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
                _inputFramePaths.push_back(path);
                _inputFrameNums.push_back(frameNum);
            }
        }
    }

    // Get filepaths for keyframes
    fs::directory_iterator keyframesIt(_keyframesDir);
    for (fs::directory_entry const& entry : keyframesIt) {
        if (!entry.is_regular_file()) {
            continue;
        }
        fs::path const& path = entry.path();

        if (std::regex_match(path.filename().string(), numberFiles)) {
            int frameNum = std::stoi(path.stem());

            // Check if within frame number range
            if (frameNum >= _begFrame && (_endFrame == -1 || frameNum <= _endFrame)) {
                _keyframePaths.push_back(path);
                _keyframeNums.push_back(frameNum);
            }
        }
    }

    auto sorter = 
         [](const fs::path& s1, const fs::path& s2) {
        std::string s1stem = s1.stem();
        std::string s2stem = s2.stem();
            return std::lexicographical_compare(s1stem.begin(), s1stem.end(), s2stem.begin(), s2stem.end());
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
void IOHandler::loadInputData(std::vector<std::shared_ptr<QImage>>& inputFrames,
		                      std::vector<std::shared_ptr<QImage>>& keyframes)
{
    // Import input frames
    for (fs::path& inputFramePath : _inputFramePaths) {
        inputFrames.push_back(std::shared_ptr<QImage>(new QImage(QString::fromStdString(inputFramePath))));
    }

    // Import keyframes
    for (fs::path& keyframePath : _keyframePaths) {
        keyframes.push_back(std::shared_ptr<QImage>(new QImage(QString::fromStdString(keyframePath))));
    }
}

fs::path IOHandler::getOneKey(){
    return _keyframePaths.at(0);
}

void IOHandler::addKey(std::vector<std::shared_ptr<QImage>>& keyframes, std::string newKey){
    keyframes.push_back(std::shared_ptr<QImage>(new QImage(QString::fromStdString(newKey))));
}

// Defaults to using the index of the image in "images" as the
// the filename of the exported image.
void IOHandler::exportImages(const std::vector<std::shared_ptr<QImage>>& images,
                             const fs::path outputDir)
{
    int padSize = calcNumDigits(images.size());

    std::vector<fs::path> filenames;

    for (uint i = 0; i < images.size(); ++i) {
        QString filename = QString::number(i).rightJustified(padSize, '0');
        filename.append(".jpg");

        filenames.push_back(fs::path(filename.toStdString()));
    }

    exportImages(images, outputDir, filenames);
}

// General exporting method for images
void IOHandler::exportImages(const std::vector<std::shared_ptr<QImage>>& images, 
                             const fs::path outputDir,
                             const std::vector<fs::path>& filenames)
{
    if (images.size() != filenames.size()) {
        std::cerr << "Error: Number of images to export does not equal "
                  << "number of filenames provided." << std::endl;
        return;
    }

    if (!fs::exists(outputDir)) {
	    fs::create_directory(outputDir);
    }
    fs::path outPath;
    // Export all images
    for (uint i = 0; i < images.size(); ++i) {
	fs::path filename = filenames.at(i);
    filename.replace_extension(".png");
	outPath = outputDir;
    outPath /= filename;
	std::cout << outPath << std::endl;
    std::cout << images.at(i)->save(QString::fromStdString(outPath), "PNG") << " for image " << i << std::endl;
    }
}


// Defaults to using _outputDir as the output directory.
void IOHandler::exportAllFrames(const std::vector<std::shared_ptr<QImage>>& images)
{
    exportAllFrames(images, _outputDir);
}

// This function is meant for exporting the same number of images as input frames,
// using the names/numbers of these frames as the filenames of the exported images.
void IOHandler::exportAllFrames(const std::vector<std::shared_ptr<QImage>>& images, const fs::path outputDir)
{
    if (images.size() != _inputFrameNums.size()) {
        std::cerr << "Error: Number of images to export does "
                  << "not equal number of input frames." << std::endl;
        return;
    }

    int padSize = calcNumDigits(images.size());

    std::vector<fs::path> filenames;

    // Export all images
    for (uint i = 0; i < images.size(); ++i) {
        QString filename = QString::number(_inputFrameNums.at(i)).rightJustified(padSize, '0');
        filename.append(".jpg");

        filenames.push_back(fs::path(filename.toStdString()));
    }
    exportImages(images, outputDir, filenames);
}

// Get the frame number associated to the input frame at index frameIdx
int IOHandler::getInputFrameNum(int frameIdx)
{
    return _inputFrameNums.at(frameIdx);
}

// Get vector of all frame numbers associated to the input frames
std::vector<int> IOHandler::getInputFrameNums()
{
    return _inputFrameNums;
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

fs::path IOHandler::exportGuide(Sequence &s, int frameNum, Guide &g)
{
    assert(frameNum >= _begFrame && frameNum <= _endFrame);
    fs::path guide = s.guideDir;
    if (!fs::exists(guide)) {
        fs::create_directory(guide);
    }
    int padSize = calcNumDigits(_inputFrameNums.size());
    guide /= g.getType() + QString::number(frameNum).rightJustified(padSize, '0').toStdString()  + ".jpg";
    g.getGuide()->save(QString::fromStdString(guide), "JPG");
    return guide;
}

fs::path IOHandler::getInputPath(Sequence &s, int frameNum)
{
    assert(frameNum >= _begFrame && frameNum <= _endFrame);
    return  _inputFramePaths.at(frameNum - _begFrame);
}

fs::path IOHandler::getOutputPath(Sequence &s, int frameNum)
{
   fs::path frame = s.outputDir;
   frame /= QString::number(frameNum).rightJustified(s.numDigits, '0').toStdString()  + ".png";

   return frame;
}

fs::path IOHandler::getErrorPath(Sequence &s, int frameNum)
{
    fs::path error = s.outputDir;
    error /=  QString::number(frameNum).rightJustified(s.numDigits, '0').toStdString() + ".bin";

    return error;
}

fs::path IOHandler::getFlowPath(int frameNum)
{
    std::cout << "frameNum " << frameNum << std::endl;
    fs::path flow = _outputDir;
    int padSize = calcNumDigits(_inputFrameNums.size());
    flow /= QString::number(frameNum).rightJustified(padSize, '0').toStdString() + ".matbin";

    return flow;
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

fs::path IOHandler::getBinaryLocation() const
{
    return _binaryLocation;
}

// Forms Sequence struct from instance variables + sequence parameters
Sequence IOHandler::makeSequence(int begFrame, int endFrame, int step, int keyframeIdx)
{
    Sequence s;
    s.begFrame = begFrame;
    s.endFrame = endFrame;
    int keyframeNum = getKeyframeNum(keyframeIdx);

    s.outputDir = _outputDir;
    int padSize = calcNumDigits(_inputFrameNums.size());
    s.outputDir /= QString::number(keyframeNum).rightJustified(padSize, '0').toStdString();

    if (!fs::exists(s.outputDir)) {
        fs::create_directory(s.outputDir);
    }
    s.guideDir = s.outputDir;
    s.guideDir /= "guides";
    if (!fs::exists(s.guideDir)) {
        fs::create_directory(s.guideDir);
    }

    s.keyframePath = _keyframePaths.at(keyframeIdx);
    s.step = step;
    s.keyframeIdx = keyframeIdx;
    s.numDigits = padSize;
    s.size = std::abs(begFrame - endFrame);

    return s;
}

// Calculates sequences to be stylized by a certain keyframe
std::vector<Sequence> IOHandler::getSequences(int keyframeIdx) {
    if (keyframeIdx >= _keyframeNums.size() || keyframeIdx < 0) {
        std::cerr << "Error: Keyframe index out of range" << std::endl;
        return std::vector<Sequence>();
	}

    int keyframeNum = _keyframeNums.at(keyframeIdx);
//    std::cout << keyframeNum << _begFrame << std::endl;
    if (keyframeNum == _begFrame) {
		if (_keyframeNums.size() == 1) {
            return std::vector<Sequence>({makeSequence(_begFrame, _endFrame, 1, keyframeIdx)});
		} else {
            return std::vector<Sequence>({makeSequence(_begFrame, _keyframeNums.at(keyframeIdx + 1), 1, keyframeIdx)});
		}
    } else if (keyframeNum == _endFrame) {
		if (_keyframeNums.size() == 1) {
            return std::vector<Sequence>({makeSequence(_endFrame, _begFrame, -1, keyframeIdx)});
        } else {
            return std::vector<Sequence>({makeSequence(_endFrame, _keyframeNums.at(keyframeIdx - 1), -1, keyframeIdx)});
        }
    } else {
            if (_keyframeNums.size() == 1) {
                  return std::vector<Sequence>({makeSequence(keyframeNum, _begFrame, -1, keyframeIdx),
                                                                    makeSequence(keyframeNum, _endFrame, 1, keyframeIdx)});
             } else if (keyframeIdx == 0) {
//                std::cout << "hi" << std::endl;
                return std::vector<Sequence>({makeSequence(keyframeNum, _begFrame, -1, keyframeIdx),
                                                                  makeSequence(keyframeNum, _keyframeNums.at(keyframeIdx + 1), 1, keyframeIdx)});
             } else if (keyframeIdx == _keyframeNums.size() - 1) {
                return std::vector<Sequence>({makeSequence(keyframeNum, _keyframeNums.at(keyframeIdx - 1), -1, keyframeIdx),
                                                                  makeSequence(keyframeNum, _endFrame, 1, keyframeIdx)});
            } else {
                return std::vector<Sequence>({makeSequence(keyframeNum, _keyframeNums.at(keyframeIdx - 1), -1, keyframeIdx),
                                                                  makeSequence(keyframeNum, _keyframeNums.at(keyframeIdx + 1), 1, keyframeIdx)});
            }
        }
}


