#include "stylizer.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencvutils.h"
#include "optical-flow/simpleflow.h"
#include "serialize.h"
#include "histogramblend.h"
#include "gradientblend.h"
#include "fft_fsolver.h"
#include <iostream>
#include <QDebug>
#include <QFileInfo>
#define GENERATE false

using namespace std;

Stylizer::Stylizer(std::vector<shared_ptr<QImage>> inputFrames, std::vector<shared_ptr<QImage>> keyFrames, IOHandler &io) :
	m_frames(inputFrames), m_keys(keyFrames), m_io(io)
{
	m_advects.reserve(m_frames.size());
}

Stylizer::~Stylizer(){
	m_frames.clear();
	m_keys.clear();
	m_advects.clear();
}

void Stylizer::run(){
	HistogramBlender hb;
	GradientBlender gb;
    std::vector<shared_ptr<QImage>> output;
    std::vector<fs::path> outfiles;
    // Load in all Sequences, generate them, and then blend
    std::vector<Sequence> seqs;
    for (uint i = 0; i < m_keys.size(); i++){
        seqs = m_io.getSequences(i);
        m_seqs.insert(m_seqs.end(), seqs.begin(), seqs.end());
    }
    Sequence cur;
    for (uint i = 0; i < m_seqs.size(); i++) {
        cur = m_seqs.at(i);
    }
    if (m_keys.size() == 1) {
        return;
    }
    int i = 0;
    Sequence a, b;
    while (i < m_seqs.size()) {
        if ((i < m_seqs.size() - 1 && m_seqs.at(i).step == m_seqs.at(i + 1).step) || i == m_seqs.size() - 1) {
            i++;
            continue;
        }
        a = m_seqs.at(i);
        b = m_seqs.at(i + 1);
        assert(a.step != b.step && a.begFrame == b.endFrame && a.endFrame == b.begFrame);
        copyKeyframes(a, b);
		std::vector<cv::Mat> masks = createMasks(a, b);
		std::vector<cv::Mat> final_masks = tempCoherence(masks);

		// histogram-preserving blend
		std::vector<cv::Mat> outBlend;
		outBlend.reserve(final_masks.size());
        hb.blend(a, b, final_masks, outBlend);

		// mask-based gradient blending
		std::vector<cv::Mat3f> gradBlendX, gradBlendY;
		gradBlendX.reserve(final_masks.size());
		gradBlendY.reserve(final_masks.size());
        gb.blend(a, b, final_masks, gradBlendX, gradBlendY);

		// poisson blend
		std::vector<cv::Mat> final_blends;
		final_blends.reserve(final_masks.size());
		poissonBlend(outBlend, gradBlendX, gradBlendY, final_blends);
		QImage out;
		for (uint i = 0; i < final_masks.size(); ++i) {
			cv::cvtColor(final_blends.at(i), final_blends.at(i), cv::COLOR_BGR2RGB);
            out = mat_to_qimage_ref(final_blends.at(i), QImage::Format::Format_RGB888).copy();
			output.push_back(std::make_shared<QImage>(out));
            outfiles.push_back(fs::path(QString::number(i + a.begFrame).rightJustified(a.numDigits, '0').toStdString()));
        }
        i+=2;
    }
    m_io.exportImages(output, a.outputDir.parent_path(), outfiles);
}

void Stylizer::copyKeyframes(Sequence& a, Sequence& b) {
    m_io.exportImages(std::vector<std::shared_ptr<QImage>>({m_keys.at(a.keyframeIdx)}), b.outputDir, std::vector<fs::path>({a.keyframePath.filename()}));
    m_io.exportImages(std::vector<std::shared_ptr<QImage>>({m_keys.at(b.keyframeIdx)}), a.outputDir, std::vector<fs::path>({b.keyframePath.filename()}));
}


void Stylizer::poissonBlend(std::vector<cv::Mat> &hp_blends, const std::vector<cv::Mat3f> &gradX, const std::vector<cv::Mat3f> &gradY, std::vector<cv::Mat> &final_blends) {
	cv::Mat3b currentFrame;
	for (uint i = 0; i < hp_blends.size(); ++i) {
		currentFrame = static_cast<cv::Mat3b>(hp_blends.at(i));
		fourierSolve(currentFrame, gradX.at(i), gradY.at(i), 0.1);
		final_blends.push_back(currentFrame);
	}
}	

std::vector<cv::Mat> Stylizer::tempCoherence(std::vector<cv::Mat> masks){
	std::vector<cv::Mat> final_masks;
	final_masks.reserve(masks.size());
	int width = masks[0].cols;
	int height = masks[0].rows;
	cv::Size size(width, height);
	Advector advector = Advector();
	cv::Mat prevMask = masks[0];

	// first mask should be all from first keyframe
	final_masks.push_back(Mat::zeros(size, CV_8UC1));

	// go through all masks that aren't first or last (keyframes)
	for (uint i=1; i < masks.size()-1; i++){
		Mat currMask = masks[i];
		// load advection field from binary or fetch from stored vector
		Mat2f flowField;
		if (GENERATE) {
			flowField = m_advects[i-1];
		} else {
                        flowField = deserializeMatbin(m_io.getFlowPath(i));
		}
		Mat advected(size, CV_8UC1);
		advector.advectMask(flowField, prevMask, advected);

		//if pixel black after advection but white in next mask, change to white
		for (int r = 0; r < height; ++r) {
			for (int c = 0; c < width; ++c) {
				if (advected.at<uchar>(r,c) == 0 && currMask.at<uchar>(r,c) == 1){
					advected.at<uchar>(r,c) = 1;
				}
			}
		}
		prevMask = advected;
		advected.convertTo(advected, CV_8UC1);
		final_masks.push_back(advected);
	}
	// last mask all from next keyframe
	final_masks.push_back(Mat::ones(size, CV_8UC1));
	return final_masks;
}

std::vector<cv::Mat> Stylizer::createMasks(Sequence& a, Sequence &b){
    assert(a.step == 1 && b.step == -1 && a.begFrame == b.endFrame && a.endFrame == b.begFrame);
	std::vector<cv::Mat> masks;
	int width = m_frames[0]->width();
	int height = m_frames[0]->height();
	cv::Size size(width, height);
    masks.reserve((a.endFrame - a.begFrame)+2);
	masks.push_back(Mat::zeros(size, CV_8UC1));

	// goes through all frames that are not keyframes
    for (uint frame= a.begFrame+a.step; frame < a.endFrame; frame+= a.step){
        std::vector<float> error_a = loadError(m_io.getErrorPath(a, frame));
        std::vector<float> error_b =  loadError(m_io.getErrorPath(b, frame));
		Mat mask(size, CV_8UC1);
		uint8_t *maskData = mask.data;

		// if error is lower from frame a, store black, otherwise white
		for (uint i = 0; i < error_a.size(); i++){
			maskData[i] = error_a[i] < error_b[i] ? 0 : 1;
		}
		mask.convertTo(mask, CV_8UC1);
		masks.push_back(mask);
	}
	masks.push_back(Mat::ones(size, CV_8UC1));
	return masks;
}

// used to load in binary files containing error values for patchmatch
std::vector<float> Stylizer::loadError(fs::path binary) {
	std::vector<float> out;
    std::ifstream in =  std::ifstream(binary, std::ifstream::binary);
	deserialize(in, out);
	return out;
}

void Stylizer::generateGuides(shared_ptr<QImage> keyframe, Sequence& s) {
	std::shared_ptr<QImage> key(new QImage(*keyframe));
    std::shared_ptr<QImage> mask(new QImage(*m_frames.at(s.begFrame)));
    std::shared_ptr<QImage> frame1(new QImage(*m_frames.at(s.begFrame)));

	// get initial GEdge guide
	GEdge edge(frame1);
    fs::path edge_initial = fs::absolute(m_io.exportGuide(s, s.begFrame, edge));

	// filler mask for advection
	mask->fill(Qt::white);
	GPos gpos_start = GPos(mask);
    fs::path pos_initial = fs::absolute(m_io.exportGuide(s, s.begFrame, gpos_start));
	GPos gpos_cur = gpos_start;
	Mat i1, i2;

	// use keyframe as initial previously stylized frame
    fs::path temp_initial = fs::absolute(s.keyframePath);
    std::shared_ptr<QImage> prevStylizedFrame(new QImage(*keyframe));

    // export keyframe so we have it
    m_io.exportImages(std::vector<std::shared_ptr<QImage>>({prevStylizedFrame}), s.outputDir, std::vector<fs::path>({s.keyframePath.filename()}));

	GTemp gtemp;

    // initial frame of video
    fs::path color_initial = fs::absolute(m_io.getInputPath(s, s.begFrame));

	// going either forwards or backwards depending on keyframe
    for (int i = s.begFrame+s.step; i != s.endFrame; i+=s.step){
        std::shared_ptr<QImage> cur_frame(new QImage(*m_frames.at(i)));

        edge.updateFrame(cur_frame);
        fs::path edge_cur = fs::absolute(m_io.exportGuide(s, i, edge));

        i1 = qimage_to_mat_ref((*m_frames.at(i-s.step)));
		i2 = qimage_to_mat_ref((*m_frames.at(i)));

		cvtColor(i1, i1, COLOR_BGRA2BGR);
		cvtColor(i2, i2, COLOR_BGRA2BGR);

		Mat2f out = calculateFlow(i1, i2, false, false);

		// if running through whole pipeline, store advection field
        if (s.step > 0) {
            serializeMatbin(out, m_io.getFlowPath(i));

			m_advects.push_back(out);
		}

		// get GPos and GTemp guides
		gpos_cur.advect(mask, out);
        fs::path pos_cur = fs::absolute(m_io.exportGuide(s, i, gpos_cur));

		gtemp.updateGuide(prevStylizedFrame, out, mask);
        fs::path temp_cur = fs::absolute(m_io.exportGuide(s, i, gtemp));

        // get current frame of video
        fs::path color_cur = fs::absolute(m_io.getInputPath(s, i));

		// build command to call ebsynth
        std::string command = m_io.getBinaryLocation().string() + " -style " + fs::absolute(s.keyframePath).string();
        command += " -guide " + edge_initial.string() +  " " + edge_cur.string() + " -weight 0.5 ";

		//        command.append("-guide " + g_mask1 + " " + g_mask2 + " -weight 6 ");

        command+="-guide " + color_initial.string() +  " " + color_cur.string() + " -weight 6 ";

        command+="-guide " + pos_initial.string() + " " + pos_cur.string() + " -weight 2 ";

        command+="-guide " + temp_initial.string() + " " + temp_cur.string() + " -weight 0.5 ";

        command+=("-output " + fs::absolute(m_io.getOutputPath(s, i)).string());

        command+=" -searchvoteiters 12 -patchmatchiters 6";

        const char *c_str = command.c_str();
		// actually calls ebsynth executable
        std::system(c_str);
        QImage prevStylized;
        Mat intermediate = cv::imread(fs::relative(m_io.getOutputPath(s, i)).string());
        cv::cvtColor(intermediate, intermediate, cv::COLOR_BGR2RGB);
        prevStylized = QImage(intermediate.data, intermediate.cols, intermediate.rows, intermediate.step, QImage::Format_RGB888).copy();
        prevStylizedFrame = std::make_shared<QImage>(prevStylized);
	}
}
