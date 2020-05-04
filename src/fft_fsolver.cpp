#include "fft_fsolver.h"
#include "fftw3.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#define NUM_OF_THREADS 1 //set number of threads to number of available processors

/*
 * The following 100 line function is our entire FFT based solver for the screened Poisson equation.
 * This function won't compile without our Image class (not included but you could replace it with your own) and the fftw library (http://www.fftw.org/).
 */


void fourierSolve(cv::Mat3b& imgData, const cv::Mat3f& imgGradX, const cv::Mat3f& imgGradY, float dataCost)
{
    cv::Mat gx, gy;

//    std::vector<cv::Mat> xChannels(3);
//    std::vector<cv::Mat> yChannels(3);

//    cv::split(imgGradX, xChannels);
//    cv::split(imgGradY, yChannels);


    cv::normalize(imgGradX.reshape(1), gx, 1, 0, cv::NORM_MINMAX);
    cv::normalize(imgGradY.reshape(1), gy, 1, 0, cv::NORM_MINMAX);

    cv::Mat xColor, yColor;
//    double min,max;
//    cv::minMaxLoc(gx, &min, &max);
//    std::cout << "gx: " << gx << std::endl;

    std::vector<float> gradX;
    if (gx.isContinuous()) {
      // array.assign((float*)mat.datastart, (float*)mat.dataend); // <- has problems for sub-matrix like mat = big_mat.row(i)
      gradX.assign((float*)gx.data, (float*)gx.data + gx.total());
    } else {
      for (int i = 0; i < gx.rows; ++i) {
        gradX.insert(gradX.end(), gx.ptr<float>(i), gx.ptr<float>(i)+gx.cols);
      }
    }
    std::cout << "channels: " << imgData.channels() << std::endl;

    std::cout << "gradient x size: " << gradX.size() << " expected size: " << std::to_string(imgData.rows * imgData.cols * imgData.channels()) << std::endl;

    std::vector<float> gradY;
    if (gy.isContinuous()) {
      // array.assign((float*)mat.datastart, (float*)mat.dataend); // <- has problems for sub-matrix like mat = big_mat.row(i)
      gradY.assign((float*)gy.data, (float*)gy.data + gy.total());
    } else {
      for (int i = 0; i < imgGradY.rows; ++i) {
        gradY.insert(gradY.end(), gy.ptr<float>(i), gy.ptr<float>(i)+gy.cols);
      }
    }
    std::cout << "gradient y size: " << gradY.size() << " expected size: " << std::to_string(imgData.rows * imgData.cols * imgData.channels()) << std::endl;


    int retVal = fftwf_init_threads();
    assert(retVal != 0);

    int nodeCount   = imgData.rows * imgData.cols;
    assert(nodeCount > 0);

    float* fftBuff   = (float*) fftwf_malloc(sizeof(*fftBuff)   * nodeCount);
    assert(fftBuff   != NULL);

    //compute two 1D lookup tables for computing the DCT of a 2D Laplacian on the fly
    float* ftLapY = (float*) fftwf_malloc(sizeof(*ftLapY) * imgData.rows);
    float* ftLapX = (float*) fftwf_malloc(sizeof(*ftLapX) * imgData.cols);
    assert(ftLapX != NULL);
    assert(ftLapY != NULL);

    for(int x = 0; x < imgData.cols; x++)
    {
        ftLapX[x] = 2.0f * cos(CV_PI * x / (imgData.cols- 1));
    }
    for(int y = 0; y < imgData.rows; y++)
    {
        ftLapY[y] = -4.0f + (2.0f * cos(CV_PI * y / (imgData.rows - 1)));
    }

    //Create a DCT-I plan for, which is its own inverse.
    fftwf_plan_with_nthreads(NUM_OF_THREADS);
    fftwf_plan fftPlan;
    fftPlan = fftwf_plan_r2r_2d(imgData.rows, imgData.cols,
                                 fftBuff, fftBuff,
                                 FFTW_REDFT00, FFTW_REDFT00, FFTW_ESTIMATE); //use FFTW_PATIENT when plan can be reused

    for(int iChannel = 0; iChannel < imgData.channels(); iChannel++)
    {
        printf("Solving channel - %i\n", iChannel);

        int nodeAddr        = 0;
        int pixelAddr       = iChannel;
        int rightPixelAddr  = imgData.channels() + iChannel;
        int topPixelAddr    = (imgData.cols * imgData.channels()) + iChannel;

        float dcSum = 0.0f;

        // compute h_hat from u, gx, gy (see equation 48 in the paper), as well as the DC term of u's DCT.
        for(int y = 0; y < imgData.rows; y++)
        for(int x = 0; x < imgData.cols;  x++,
            nodeAddr++, pixelAddr += imgData.channels(), rightPixelAddr += imgData.channels(), topPixelAddr += imgData.channels())
        {
            // Compute DC term of u's DCT without computing the whole DCT.
            float dcMult = 1.0f;
            if((x > 0) && (x < imgData.cols - 1))
                dcMult *= 2.0f;
            if((y > 0) && (y < imgData.rows - 1))
                dcMult *= 2.0f;
            dcSum += dcMult * static_cast<float>(imgData.data[pixelAddr])/255.0f;

            fftBuff[nodeAddr] = dataCost * static_cast<float>(imgData.data[pixelAddr])/255.0f;;

            // Subtract g^x_x and g^y_y, with boundary factor of -2.0 to account for boundary reflections implicit in the DCT
            if((x > 0) && (x < imgData.cols - 1))
                fftBuff[nodeAddr] -= (gradX[rightPixelAddr] - gradX[pixelAddr]);
            else
                fftBuff[nodeAddr] -= (-2.0f * gradX[pixelAddr]);

            if((y > 0) && (y < imgData.rows - 1))
                fftBuff[nodeAddr] -= (gradY[topPixelAddr] - gradY[pixelAddr]);
            else
                fftBuff[nodeAddr] -= (-2.0f * gradY[pixelAddr]);
        }

        //transform h_hat to H_hat by taking the DCT of h_hat
        fftwf_execute(fftPlan);

        //compute F_hat using H_hat (see equation 29 in the paper)
        nodeAddr = 0;
        for(int y = 0; y < imgData.rows; y++)
        for(int x = 0; x < imgData.cols;  x++, nodeAddr++)
        {
            float ftLapResponse = ftLapY[y] + ftLapX[x];
            fftBuff[nodeAddr] /= (dataCost - ftLapResponse);
        }

        /*
         * Set the DC term of the solution to the value computed above (i.e., the DC term of imgData).
         * When dataCost = 0 (i.e., there is no data image and the problem becomes pure gradient field integration)
         * then the DC term  of the solution is undefined. So if you want to control the DC of the solution
         * when dataCost = 0 then before calling fourierSolve() set every pixel in 'imgData' to the average value
         * you would like the pixels in the solution to have.
         */
        fftBuff[0] = dcSum;

        //transform F_hat to f_hat by taking the inverse DCT of F_hat
        fftwf_execute(fftPlan);
        float fftDenom = 4.0f * (imgData.cols- 1) * (imgData.rows - 1);
        pixelAddr = iChannel;
        for(int iNode = 0; iNode < nodeCount; iNode++, pixelAddr += imgData.channels())
        {
            imgData.data[pixelAddr] = static_cast<uchar>(std::min(255.0f, std::max(fftBuff[iNode] / fftDenom * 255.0f, 0.0f)));
        }
    }

    fftwf_free(fftBuff);
    fftwf_free(ftLapX);
    fftwf_free(ftLapY);
    fftwf_destroy_plan(fftPlan);
    fftwf_cleanup_threads();

    printf("\tDone.\n");
}
