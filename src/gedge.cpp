#include "gedge.h"
#include <QCoreApplication>
#include <QImage>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#define SIGMA 6.0
#define LAP false //option with Laplacian
#if LAP==true //if using Laplacian filter
#define SIZE 3
#endif
#if LAP==false //if using Gaussian filter instead
#define SIZE 5
#endif
#define SAVE true //saves guide as image in directory

using namespace cv;

GEdge::GEdge(std::shared_ptr<QImage> currFrame) :
    Guide(currFrame),
    m_guide(nullptr)
{
    createEdge(currFrame);
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

    if (LAP){ //create 3x3 Laplacian filter
        GKernel.push_back(0);
        GKernel.push_back(1);
        GKernel.push_back(0);

        GKernel.push_back(1);
        GKernel.push_back(-4);
        GKernel.push_back(1);

        GKernel.push_back(0);
        GKernel.push_back(1);
        GKernel.push_back(0);
    } else { //otherwise generate 5x5 Gaussian filter
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
}

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
            RGBA color;
            if (LAP){ //add 0.5 to all values
                color = {REAL2byte(red_acc+0.5), REAL2byte(green_acc+0.5), REAL2byte(blue_acc+0.5)};
            } else {
                color = {REAL2byte(red_acc), REAL2byte(green_acc), REAL2byte(blue_acc)};
            }
            temp[index] = color;
        }
    }

    if (LAP){
        memcpy(data, &temp[0], width*height*sizeof(RGBA));
    } else {
        std::vector<RGBA> temp2;
        temp2.reserve(width*height);

        RGBA add = {REAL2byte(0.5), REAL2byte(0.5), REAL2byte(0.5)};

        //for Gaussian, subtract blurred from original and add 0.5
        for(int i=0; i < height; i++){
            for(int j=0; j < width; j++){
                size_t index = i * width + j;
                temp2[index] = data[index] - temp[index];
                temp2[index] = temp2[index] + add;
            }
        }
        memcpy(data, &temp2[0], width*height*sizeof(RGBA));
    }
}

// convert image to grayscale
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
    //convert to 4 channel RGBA
    if (currFrame->format() != QImage::Format_RGBX8888){
        currFrame = std::make_shared<QImage>(currFrame->convertToFormat(QImage::Format_RGBX8888));
    }
    RGBA *data = getData(currFrame);
    const int height = currFrame->height();
    const int width = currFrame->width();

    std::vector<double> GKernel;
    GKernel.reserve(SIZE*SIZE);

    makeFilter(GKernel);
    makeGray(data, width, height);
    convolve(GKernel, data, width, height);

    if (SAVE){
        const QString filename("test.png");
        currFrame->save(filename, nullptr, 100);
    }
    m_guide = currFrame;
}
