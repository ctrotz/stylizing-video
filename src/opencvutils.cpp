#include "opencvutils.h"
#include <stdio.h>
#include <stdarg.h>
cv::Mat qimage_to_mat_ref(QImage &image)
{
	cv::Mat mat;
	switch (image.format())
	{
		case QImage::Format_ARGB32:
		case QImage::Format_RGB32:
		case QImage::Format_ARGB32_Premultiplied:
			mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
			break;
		case QImage::Format_RGB888:
			mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
			cv::cvtColor(mat, mat, COLOR_BGR2RGB);
			break;
		case QImage::Format_Grayscale8:
			mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
			break;
	}
	return mat;
}

QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format)
{
	return QImage(mat.data, mat.cols, mat.rows, mat.step1(), format);
}

String type2str(int type) {
	String r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch ( depth ) {
		case CV_8U:  r = "8U"; break;
		case CV_8S:  r = "8S"; break;
		case CV_16U: r = "16U"; break;
		case CV_16S: r = "16S"; break;
		case CV_32S: r = "32S"; break;
		case CV_32F: r = "32F"; break;
		case CV_64F: r = "64F"; break;
		default:     r = "User"; break;
	}

	r += "C";
	r += (chans+'0');

	return r;
}

// source: https://stackoverflow.com/questions/16312904/how-to-write-a-float-mat-to-a-file-in-opencv
void serializeMatbin(cv::Mat& mat, std::string filename){
	if (!mat.isContinuous()) {
		std::cout << "Not implemented yet" << std::endl;
		exit(1);
	}

	int elemSizeInBytes = (int)mat.elemSize();
	int elemType        = (int)mat.type();
	int dataSize        = (int)(mat.cols * mat.rows * mat.elemSize());

	FILE* FP = fopen(filename.c_str(), "wb");
	int sizeImg[4] = {mat.cols, mat.rows, elemSizeInBytes, elemType };
	fwrite(/* buffer */ sizeImg, /* how many elements */ 4, /* size of each element */ sizeof(int), /* file */ FP);
	fwrite(mat.data, mat.cols * mat.rows, elemSizeInBytes, FP);
	fclose(FP);
}

cv::Mat deserializeMatbin(std::string filename){
	FILE* fp = fopen(filename.c_str(), "rb");
	int header[4];
	fread(header, sizeof(int), 4, fp);
	int cols            = header[0];
	int rows            = header[1];
	int elemSizeInBytes = header[2];
	int elemType        = header[3];

	cv::Mat outputMat = cv::Mat::ones(rows, cols, elemType);

	size_t result = fread(outputMat.data, elemSizeInBytes, (size_t)(cols * rows), fp);

	if (result != (size_t)(cols * rows)) {
		fputs ("Reading error", stderr);
	}

	fclose(fp);
	return outputMat;
}

