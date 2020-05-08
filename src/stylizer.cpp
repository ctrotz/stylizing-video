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

	for (uint i = 0; i < m_keys.size()-1; i++){
		// store key paths to pass into histogram blender
		int keyNum_a =  m_io.getKeyframeNum(i);
		QString keynum_a = QString::number(keyNum_a).rightJustified(3, '0');
		QString key_a("./data/test/keys/" + keynum_a + ".jpg");
		int keyNum_b =  m_io.getKeyframeNum(i+1);
		QString keynum_b = QString::number(keyNum_b).rightJustified(3, '0');
		QString key_b("./data/test/keys/" + keynum_b + ".jpg");

		int beg = m_io.getKeyframeNum(i);
		int end = m_io.getKeyframeNum(i+1);

		// treat beg/end as indices for input frames
		std::pair<std::vector<QString>, std::vector<QString>> a;
		std::pair<std::vector<QString>, std::vector<QString>> b;

		// Only run generateGuides if you don't have the stylized frames saved to file!
		if (GENERATE){
			a = generateGuides(m_keys.at(i), i, beg, end, 1);
			b = generateGuides(m_keys.at(i+1), i+1, end, beg, -1);
		} else {
			a = fetchGuides(i, beg, end, 1);
			b = fetchGuides(i+1, end, beg, -1);
		}

		// Files were pushed onto b in reverse order
		std::reverse(b.first.begin(), b.first.end());
		std::reverse(b.second.begin(), b.second.end());

		// blend a and b
		std::vector<cv::Mat> masks = createMasks(a, b);
		std::vector<cv::Mat> final_masks = tempCoherence(masks);

		// add keys to beg/end of stylized frames
		a.first.insert(a.first.begin(), key_a);
		a.first.push_back(key_b);
		b.first.insert(b.first.begin(), key_a);
		b.first.push_back(key_b);

		// histogram-preserving blend
		std::vector<cv::Mat> outBlend;
		outBlend.reserve(final_masks.size());
		hb.blend(a.first, b.first, final_masks, outBlend);

		// mask-based gradient blending
		std::vector<cv::Mat3f> gradBlendX, gradBlendY;
		gradBlendX.reserve(final_masks.size());
		gradBlendY.reserve(final_masks.size());
		gb.blend(a.first, b.first, final_masks, gradBlendX, gradBlendY);

		// poisson blend
		std::vector<cv::Mat> final_blends;
		final_blends.reserve(final_masks.size());
		poissonBlend(outBlend, gradBlendX, gradBlendY, final_blends);

		std::vector<shared_ptr<QImage>> output;
		output.reserve(final_masks.size());
		QImage out;
		for (uint i = 0; i < final_masks.size(); ++i) {
			cv::cvtColor(final_blends.at(i), final_blends.at(i), cv::COLOR_BGR2RGB);
			out = mat_to_qimage_ref(final_blends.at(i), QImage::Format::Format_RGB888);
			output.push_back(std::make_shared<QImage>(out));
		}	
		m_io.exportAllFrames(output);

	}
}

void Stylizer::poissonBlend(std::vector<cv::Mat> &hp_blends, const std::vector<cv::Mat3f> &gradX, const std::vector<cv::Mat3f> &gradY, std::vector<cv::Mat> &final_blends) {
	cv::Mat3b currentFrame;
	for (uint i = 0; i < hp_blends.size(); ++i) {
		currentFrame = static_cast<cv::Mat3b>(hp_blends.at(i));
		//cv::imshow("pre-solve", currentFrame);
		//cv::waitKey(0);
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
			flowField = deserializeMatbin(m_flowpaths[i-1]);
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

// Method used for when stylized frames are already stored on file
std::pair<std::vector<QString>, std::vector<QString>> Stylizer::fetchGuides(int keyIdx, int beg, int end, int step) {
	std::vector<QString> outpaths;
	outpaths.reserve(abs(end-beg) + 1);
	std::vector<QString> errorpaths;
	errorpaths.reserve(abs(end-beg) + 1);
	// Don't delete the commented-out code here! This code will save the advection matrices as binary files, which
	// you don't need if you have the ./flowfields/ folder downloaded locally.

	//    Mat i1, i2;
	for (int i = beg+step; i != end; i+=step){
		if (step > 0){
			//            i1 = qimage_to_mat_ref((*m_frames.at(i-step)));
			//            i2 = qimage_to_mat_ref((*m_frames.at(i)));

			//            cvtColor(i1, i1, COLOR_BGRA2BGR);
			//            cvtColor(i2, i2, COLOR_BGRA2BGR);

			//            Mat2f out = calculateFlow(i1, i2, false, false);
			std::string filename = "flowfields/" + std::to_string(i) + ".matbin";
			//            serializeMatbin(out, filename);
			//            m_advects.push_back(out);
			m_flowpaths.push_back(filename);
		}
		QString frame = QString::number(i).rightJustified(3, '0');
		QString prefix("outtest/" + QString::number(keyIdx) + "-" + frame );
		QString outfile(prefix+".png");
		outpaths.push_back(outfile);
		errorpaths.push_back(prefix + ".bin");
	}
	return {outpaths,errorpaths};
}

std::vector<cv::Mat> Stylizer::createMasks(std::pair<std::vector<QString>, std::vector<QString>> a, std::pair<std::vector<QString>, std::vector<QString>> b){
	std::vector<cv::Mat> masks;
	int width = m_frames[0]->width();
	int height = m_frames[0]->height();
	cv::Size size(width, height);
	masks.reserve(a.first.size()+2);
	masks.push_back(Mat::zeros(size, CV_8UC1));

	// goes through all frames that are not keyframes
	for (uint frame=0; frame < a.first.size(); frame++){
		std::vector<float> error_a = loadError(a.second[frame]);
		std::vector<float> error_b = loadError(b.second[frame]);
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
std::vector<float> Stylizer::loadError(QString& binary) {
	std::vector<float> out;
	std::ifstream in =  std::ifstream(binary.toStdString(), std::ifstream::binary);
	deserialize(in, out);
	return out;
}

std::pair<std::vector<QString>, std::vector<QString>> Stylizer::generateGuides(shared_ptr<QImage> keyframe, int keyIdx, int beg, int end, int step) {
	// paths for stylized frames
	std::vector<QString> outpaths;
	outpaths.reserve(abs(end-beg) + 1);
	// paths for binary files containing patch errors
	std::vector<QString> errorpaths;
	errorpaths.reserve(abs(end-beg) + 1);

	QString prevEdge;

	std::shared_ptr<QImage> key(new QImage(*keyframe));
	std::shared_ptr<QImage> mask(new QImage(*m_frames.at(beg)));
	std::shared_ptr<QImage> frame1(new QImage(*m_frames.at(beg)));

	// get initial GEdge guide
	GEdge edge(frame1);
	prevEdge = edge.getGuide(beg);

	// filler mask for advection
	mask->fill(Qt::white);
	GPos gpos_start = GPos(mask);
	GPos gpos_cur = gpos_start;
	// get initial GPos guide
	QString prevPos = gpos_cur.getGuide(beg);

	Mat i1, i2;

	// use keyframe as initial previously stylized frame
	int keyNum =  m_io.getKeyframeNum(keyIdx);
	QString keynum = QString::number(keyNum).rightJustified(3, '0');
	QString prevTemp("./data/test/keys/" + keynum + ".jpg");
	std::shared_ptr<QImage> prevStylizedFrame(new QImage(*key));
	GTemp gtemp;

	// going either forwards or backwards depending on keyframe
	for (int i = beg+step; i != end; i+=step){
		std::shared_ptr<QImage> frame2(new QImage(*m_frames.at(i)));

		edge.updateFrame(frame2);
		QString g_edge2 = edge.getGuide(i);

		i1 = qimage_to_mat_ref((*m_frames.at(i-step)));
		i2 = qimage_to_mat_ref((*m_frames.at(i)));

		cvtColor(i1, i1, COLOR_BGRA2BGR);
		cvtColor(i2, i2, COLOR_BGRA2BGR);

		Mat2f out = calculateFlow(i1, i2, false, false);

		// if running through whole pipeline, store advection field
		if (step > 0){
			m_advects.push_back(out);
		}

		// get GPos and GTemp guides
		gpos_cur.advect(mask, out);
		QString g_pos2 = gpos_cur.getGuide(i);

		gtemp.updateGuide(prevStylizedFrame, out, mask);
		QString g_temp2 = gtemp.getGuide(i);

		// get 3-digit frame number
		QString prevframe = QString::number(beg).rightJustified(3, '0');
		QString frame = QString::number(i).rightJustified(3, '0');

		// build command to call ebsynth

		QString command("cd ./deps/ebsynth && bin/ebsynth -style ../../data/test/keys/" + keynum + ".jpg ");
		command.append("-guide ../." + prevEdge + " ../." + g_edge2 + " -weight 0.5 ");

		//        command.append("-guide " + g_mask1 + " " + g_mask2 + " -weight 6 ");

		command.append("-guide ../../data/test/video/" + prevframe + ".jpg " + "../../data/test/video/" + frame + ".jpg -weight 6 ");

		command.append("-guide ../." + prevPos + " ../." + g_pos2 + " -weight 2 ");

		command.append("-guide ../." + prevTemp + " ../." + g_temp2 + " -weight 0.5 ");

		QString prefix("outtest/" + QString::number(keyIdx) + "-" + frame );

		QString outfile(prefix+".png");
		command.append("-output ../../" + outfile);

		command.append(" -searchvoteiters 12 -patchmatchiters 6");

		QByteArray ba = command.toLocal8Bit();
		const char *c_str = ba.data();

		// actually calls ebsynth executable
		std::system(c_str);

		prevStylizedFrame = std::make_shared<QImage>(outfile);

		outpaths.push_back(outfile);
		errorpaths.push_back(prefix + ".bin");
	}
	return {outpaths,errorpaths};
}
