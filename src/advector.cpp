#include "advector.h"

#include <Eigen/Dense>

#include <algorithm>
#include <iostream>

Advector::Advector()
{
}

void Advector::advect(const cv::Mat2f& flowField, std::shared_ptr<QImage> mask,
                      std::shared_ptr<QImage> inFrame, std::shared_ptr<QImage> outFrame)
{
    assert(inFrame->size() == outFrame->size());

    int imgH = outFrame->height();
    int imgW = outFrame->width();

    for (int r = 0; r < imgH; ++r) {
        for (int c = 0; c < imgW; ++c) {
            cv::Vec2f u = flowField.at<cv::Vec2f>(r, c);

            cv::Vec2f prevPos = cv::Vec2f({static_cast<float>(c), static_cast<float>(r)}) - u;

            QColor interpColor = bilinearInterpolate(inFrame, mask, prevPos);
            outFrame->setPixelColor(c, r, interpColor);
        }
    }
}

void Advector::advectMask(const cv::Mat2f& flowField, cv::Mat& inMask, cv::Mat& outMask)
{
    assert(inMask.size() == outMask.size());

    int imgH = outMask.rows;
    int imgW = outMask.cols;

    uint8_t *maskData = outMask.data;

    for (int r = 0; r < imgH; ++r) {
        for (int c = 0; c < imgW; ++c) {
            cv::Vec2f u = flowField.at<cv::Vec2f>(r, c);

            cv::Vec2f prevPos = cv::Vec2f({static_cast<float>(c), static_cast<float>(r)}) - u;

            float interpColor = bilinearInterpolateMask(inMask, prevPos);
//            std::cout << r << " " << c << " " << interpColor << std::endl;
//            std::cout << "before: " << outMask.at<float>(r,c) << std::endl;
//            outMask.at<float>(r,c) = interpColor;
            maskData[r*imgW + c] = interpColor;
//            std::cout << "after: " << outMask.at<float>(r,c) << std::endl;
        }
    }
}

float Advector::bilinearInterpolateMask(cv::Mat& img, cv::Vec2f pos)
{
    float x = pos[0];
    float y = pos[1];

    // If x or y is exactly an integer coordinate, move it a negligible
    // amount to avoid NaNs in interpolation equation
    if (x == std::floor(x)) x += 1e-4;
    if (y == std::floor(y)) y += 1e-4;

    // Algorithm of bilinear interpolation derived from:
    // https://en.wikipedia.org/wiki/Bilinear_interpolation

    int x1 = std::floor(x);
    int x2 = std::ceil(x);
    int y1 = std::floor(y);
    int y2 = std::ceil(y);

    float f11 = 0;
    float f12 = 0;
    float f21 = 0;
    float f22 = 0;

    if (isInBoundsMask(x1, y1, img)) {
        f11 = img.at<float>(x1, y1);
    }
    if (isInBoundsMask(x1, y2, img)) {
        f12 = img.at<float>(x1, y2);
    }
    if (isInBoundsMask(x2, y1, img)) {
//        std::cout << img.cols << std::endl;
        f21 = img.at<float>(x2, y1);
    }
    if (isInBoundsMask(x2, y2, img)) {
        f22 = img.at<float>(x2, y2);
    }

    float scale = 1.f / static_cast<float>((x2-x1)*(y2-y1));

    Eigen::Matrix<float, 1, 2> xMat;
    xMat << x2-x, x-x1;

    Eigen::Matrix<float, 2, 1> yMat;
    yMat << y2-y, y-y1;

    float outColor = 0;
    Eigen::Matrix<float, 2, 2> colorMat;
    colorMat << f11, f12, f21, f22;

    outColor = scale * xMat * colorMat * yMat;
    outColor = std::clamp(outColor, 0.f, 1.f);

    float returnColor = std::round(outColor);
    return returnColor;
}

QColor Advector::bilinearInterpolate(std::shared_ptr<QImage> img,
                                     std::shared_ptr<QImage> mask,
                                     cv::Vec2f pos)
{
    float x = pos[0];
    float y = pos[1];

    // If x or y is exactly an integer coordinate, move it a negligible
    // amount to avoid NaNs in interpolation equation
    if (x == std::floor(x)) x += 1e-4;
    if (y == std::floor(y)) y += 1e-4;

    // Algorithm of bilinear interpolation derived from:
    // https://en.wikipedia.org/wiki/Bilinear_interpolation

    int x1 = std::floor(x);
    int x2 = std::ceil(x);
    int y1 = std::floor(y);
    int y2 = std::ceil(y);

    QColor f11 = QColor(0,0,0);
    QColor f12 = QColor(0,0,0);
    QColor f21 = QColor(0,0,0);
    QColor f22 = QColor(0,0,0);

    if (isInBounds(x1, y1, img, mask)) {
        f11 = img->pixelColor(x1, y1);
    }
    if (isInBounds(x1, y2, img, mask)) {
        f12 = img->pixelColor(x1, y2);
    }
    if (isInBounds(x2, y1, img, mask)) {
        f21 = img->pixelColor(x2, y1);
    }
    if (isInBounds(x2, y2, img, mask)) {
        f22 = img->pixelColor(x2, y2);
    }

    float scale = 1.f / static_cast<float>((x2-x1)*(y2-y1));

    Eigen::Matrix<float, 1, 2> xMat;
    xMat << x2-x, x-x1;
    
    Eigen::Matrix<float, 2, 1> yMat;
    yMat << y2-y, y-y1;

    int f11c[3] = {f11.red(), f11.green(), f11.blue()};
    int f12c[3] = {f12.red(), f12.green(), f12.blue()};
    int f21c[3] = {f21.red(), f21.green(), f21.blue()};
    int f22c[3] = {f22.red(), f22.green(), f22.blue()};

    float outColor[3] = {0, 0, 0};

    for (int c = 0; c < 3; ++c) {
        Eigen::Matrix<float, 2, 2> colorMat;
        colorMat << f11c[c], f12c[c], f21c[c], f22c[c];

        outColor[c] = scale * xMat * colorMat * yMat;
        outColor[c] = std::clamp(outColor[c], 0.f, 255.f);
    }

    QColor returnColor (static_cast<int>(std::round(outColor[0])),
                        static_cast<int>(std::round(outColor[1])),
                        static_cast<int>(std::round(outColor[2])));

    return returnColor;
}

// Takes masks into account as well as image boundaries
bool Advector::isInBounds(int x, int y, std::shared_ptr<QImage> img,
                          std::shared_ptr<QImage> mask)
{
    if (x < 0 || x >= img->width()) return false;
    if (y < 0 || y >= img->height()) return false;
    return mask->pixel(x, y) != 0;
}

bool Advector::isInBoundsMask(int x, int y, cv::Mat& img)
{
    if (x < 0 || x >= img.cols) return false;
    if (y < 0 || y >= img.rows) return false;
    return true;
}
