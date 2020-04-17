#ifndef GEDGE_H
#define GEDGE_H

#include "guide.h"
#include <QObject>
#include "RGBA.h"


class GEdge : public Guide
{
public:
    GEdge(std::shared_ptr<QImage> currFrame);
    virtual ~GEdge();

    void makeFilter(std::vector<double> &GKernel);
    void convolve(std::vector<double> &GKernel, RGBA* data, int width, int height);
    void makeGray(RGBA *data, int width, int height);

    RGBA *getData(std::shared_ptr<QImage> image);

    unsigned char REAL2byte(float f) {
        int i = static_cast<int>((f * 255.0 + 0.5));
        return (i < 0) ? 0 : (i > 255) ? 255 : i;
    }
    // Turns an RGB value into a grayscale value
    unsigned char RGBAToGray(const RGBA &pixel) {
        return (0.299*int(pixel.r)) + (0.587*int(pixel.g)) + (0.114*int(pixel.b));
    }

protected:
    std::shared_ptr<QImage> getGuide();

private:
    std::shared_ptr<QImage> m_guide;
    void createEdge(std::shared_ptr<QImage> currFrame);
    void createEdge2(std::shared_ptr<QImage> currFrame);
};

#endif // GEDGE_H
