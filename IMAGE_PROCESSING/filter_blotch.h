#ifndef FILTER_BLOTCH_H
#define FILTER_BLOTCH_H

#include "contour_processing.h"
#include <QString>

class FilterBlotchClass
{
    public:
    void FilterImage(cv::Mat& Image);
    ContoursProcessorClass FindContours;
    std::string TAG_NAME{"[ BLOTCH_FILTER ]"};
    bool isEmpty(const cv::Rect& rect);
};

#endif 
