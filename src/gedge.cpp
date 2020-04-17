#include "gedge.h"
#include <QCoreApplication>
#include <QImage>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#define SIZE 5
#define SIGMA 6.0

using namespace cv;

GEdge::GEdge(std::shared_ptr<QImage> currFrame) :
    Guide(currFrame),
    m_guide(nullptr)
{
    createEdge2(currFrame);
}

GEdge::~GEdge(){
    m_guide = nullptr;
}

std::shared_ptr<QImage> GEdge::getGuide(){
    return m_guide;
}

RGBA* GEdge::getData(std::shared_ptr<QImage> image){
    return reinterpret_cast<RGBA *>(image->bits());
}

// https://www.geeksforgeeks.org/gaussian-filter-generation-c/
void GEdge::makeFilter(std::vector<double> &GKernel){
    double r, s = 2.0 * SIGMA * SIGMA;

    double sum = 0.0;

    int radius = floor(SIZE/2.0);

//    GKernel.push_back(0);
//    GKernel.push_back(1);
//    GKernel.push_back(0);

//    GKernel.push_back(1);
//    GKernel.push_back(-4);
//    GKernel.push_back(1);

//    GKernel.push_back(0);
//    GKernel.push_back(1);
//    GKernel.push_back(0);


    //generate kernel
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            r = sqrt(x * x + y * y);
            size_t index = ((x+radius) * SIZE) + (y+radius);
            GKernel[index] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += GKernel[index];
        }
    }

    // normalizing the kernel
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            size_t index = i * SIZE + j;
            GKernel[index] /= sum;
        }
    }
}

// http://www.songho.ca/dsp/convolution/convolution.html#cpp_conv2d
void GEdge::convolve(std::vector<double> &GKernel, RGBA* data, const int width, const int height){
    // find center position of kernel (half of kernel size)
    const int radius = floor(SIZE/2.0);

    std::vector<RGBA> temp;
    temp.reserve(width*height);

    for(int i=0; i < height; i++){ //rows

        for(int j=0; j < width; j++){ //columns

            double red_acc=0;
            double green_acc=0;
            double blue_acc=0;

            size_t index = i * width + j;

            for(int m=0; m < SIZE; m++){ //kernel rows

                size_t mm = SIZE - 1 - m;      // row index of flipped kernel

                for(int n=0; n < SIZE; n++){ // kernel columns

                    size_t nn = SIZE - 1 - n;  // column index of flipped kernel

                    // index of input signal, used for checking boundary
                    size_t ii = i + (radius - mm);
                    size_t jj = j + (radius - nn);

                    size_t bindex = ii * width + jj;
                    size_t kindex = mm * radius + nn;
                    float curr_val = GKernel[kindex];

                    // ignore input samples which are out of bound
                    if( ii >= 0 && ii < height && jj >= 0 && jj < width ){
                        red_acc += curr_val*data[bindex].r/255.0;
                        green_acc += curr_val*data[bindex].g/255.0;
                        blue_acc += curr_val*data[bindex].b/255.0;
                    }
                }
            }
            RGBA color = {REAL2byte(red_acc), REAL2byte(green_acc), REAL2byte(blue_acc)};
            temp[index] = color;
        }
    }

    std::vector<RGBA> temp2;
    temp2.reserve(width*height);

    for(int i=0; i < height; i++){ //rows
        for(int j=0; j < width; j++){ //columns
            size_t index = i * width + j;
            temp2[index] = data[index] - temp[index];
//            temp2[index] = temp[index];
        }
    }
    memcpy(data, &temp2[0], width*height*sizeof(RGBA));
}

void GEdge::makeGray(RGBA *data, const int width, const int height){
    for(int i=0; i < height; i++){
        for(int j=0; j < width; j++){
            size_t index = i * width + j;
            unsigned char gray = RGBAToGray(data[index]);
            data[index].r = gray;
            data[index].g = gray;
            data[index].b = gray;
        }
    }
}

void GEdge::createEdge(std::shared_ptr<QImage> currFrame){
    if (currFrame->format() != QImage::Format_RGBX8888){
        currFrame = std::make_shared<QImage>(currFrame->convertToFormat(QImage::Format_RGBX8888));
    }
    RGBA *data = getData(currFrame);
    const int height = currFrame->height();
    const int width = currFrame->width();
    std::cout << height << std::endl;

    std::vector<double> GKernel;
    GKernel.reserve(SIZE*SIZE);

    makeFilter(GKernel);
    makeGray(data, width, height);
    convolve(GKernel, data, width, height); //maybe fill in temp2 instead
    std::cout<< width * height << std::endl;

//    std::shared_ptr<QImage> guide(new QImage(width, height, QImage::Format_RGBX8888));
//    memset(guide->bits(), 0, width * height * sizeof(RGBA));
//    memcpy(data, guide->bits(), guide->byteCount());
//    std::cout << static_cast<int>(guide[100].r) <<std::endl;

    for(int i=0; i < height; i++){
        for(int j=0; j < width; j++){
            size_t index = i * width + j;
            int r = data[index].r;
            int g = data[index].g;
            int b = data[index].b;
            currFrame->setPixel(i,j, qRgb(r,g,b));
        }
    }
    const QString filename("test5.png");
    currFrame->save(filename, nullptr, 100);
//    m_guide = guide;
}

void GEdge::createEdge2(std::shared_ptr<QImage> currFrame){
//    QImage::Format fm = currFrame->format();
    Mat curr = cv::imread("./data/minitest/video/000.jpg");
//    Mat mat = Mat(currFrame->height(), currFrame->width(), 1, const_cast<uchar*>(currFrame->bits()), currFrame->bytesPerLine()).clone();
//    std::vector<int> compression_params;
//    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
//    compression_params.push_back(9);
//    imwrite("sad.png", mat, compression_params);
//    Mat mat(currFrame->height(), currFrame->width(), CV_16SC3, currFrame->bits());
//    Mat output;
//    GaussianBlur(mat, output, Size(3,3), 6);
//    cv::namedWindow( "Display window", WINDOW_KEEPRATIO );// Create a window for display.
//    cv::imshow( "Display window", mat);
//    waitKey(0);
//    *currFrame = QImage(output.data, output.cols, output.rows, output.step, fm).copy();
//    const QString filename("test3.png");
//    currFrame->save(filename, nullptr, 100);
//    m_guide = currFrame;
}
