#ifndef IMAGE_ESTIMATORS_H
#define IMAGE_ESTIMATORS_H

#include <opencv2/core.hpp>

class GeometryDistortionImage
{
    public:
    static float SetImage(const cv::Mat& Image, int Threshold = 40);
    static float GetResult();
    static float ImageDistortion;
    static float XMax;
    static float YMax;
};



#endif 
