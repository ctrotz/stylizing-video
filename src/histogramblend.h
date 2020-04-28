#ifndef HISTOGRAMBLEND_H
#define HISTOGRAMBLEND_H

#include <Eigen/Dense>
#include <QImage>
#include <opencv2/core/mat.hpp>

namespace histogramBlend
{

void blend(std::vector<std::shared_ptr<QImage>> &seqA,
	   std::vector<std::shared_ptr<QImage>> &seqB,
	   std::vector<std::shared_ptr<QImage>> &errMask);

};

#endif // HISTOGRAMBLEND_H
