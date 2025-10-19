#ifndef FILTER_BLOTCH_H
#define FILTER_BLOTCH_H

#include "contour_processing.h"
#include <QString>

class FilterBlotchClass
{
    public:
    void FilterImage(cv::Mat& FilteringImage, cv::Mat& RawImage);
    ContoursProcessorClass FindContours;
    std::string TAG_NAME{"[ BLOTCH_FILTER ]"};
};

#endif 
