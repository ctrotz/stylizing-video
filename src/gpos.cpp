#include "advector.h"
#include "gpos.h"
#include "gradientblend.h"
#include "opencvutils.h"
#include "opencv2/photo.hpp"
#include "fft_fsolver.h"

#include <QLinearGradient>
#include <QPainter>

GPos::GPos(std::shared_ptr<QImage> g_mask) :
    Guide(),
    m_guide(generateGradient(g_mask->width(), g_mask->height())),
    m_mask(g_mask)
{

}

GPos::~GPos(){
    m_guide = nullptr;
}

std::shared_ptr<QImage> GPos::generateGradient(int width, int height, QColor xColor, QColor yColor)
{
    std::shared_ptr<QImage> out = std::make_shared<QImage>(width, height, QImage::Format_ARGB32);
    out->fill(Qt::white);
    QLinearGradient linearGrad(QPointF(width, 0), QPointF(0, height));
    linearGrad.setColorAt(0, xColor);
    linearGrad.setColorAt(1, yColor);
    QPainter painter(&(*out));
    painter.fillRect(out->rect(), linearGrad);
    return out;
}

std::shared_ptr<QImage> GPos::getGuide(){
    return m_guide;
}

QString GPos::getGuide(int i){
    QString filename("./guides/pos");
    filename.append(QString::number(i));
    filename.append(".png");
    m_guide->save(filename, nullptr, 100);
    return filename;
}

std::string GPos::getType()
{
    return "pos";
}

void GPos::advect(std::shared_ptr<QImage> g_mask, cv::Mat2f& flowField) {
    Advector advector = Advector();
    std::shared_ptr<QImage> advected = std::make_shared<QImage>(m_guide->width(), m_guide->height(), m_guide->format());
    advected->fill(Qt::white);
    advector.advect(flowField, g_mask, m_guide, advected);
    cv::Mat gpos = qimage_to_mat_ref(*advected);
    cv::Mat gray;
    cv::cvtColor(gpos, gpos, cv::COLOR_BGRA2BGR);

    std::vector<cv::Mat> bgrChannels(3);
    cv::split(gpos, bgrChannels);

    gray = bgrChannels.at(1) + bgrChannels.at(2);
    gray *= 0.5;

    cv::Mat mask;
    cv::threshold(gray, mask, 120, 255, cv::THRESH_BINARY_INV);

    cv::inpaint(gpos, mask, gpos, 30, cv::INPAINT_NS);
    cv::cvtColor(gpos, gpos, cv::COLOR_BGR2BGRA);
    m_guide = std::make_shared<QImage>(mat_to_qimage_ref(gpos, m_guide->format()).copy());

    m_mask = g_mask;
}

