#ifndef CONTOUR_PROCESSING_H
#define CONTOUR_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <QPair>

class ContoursProcessorClass
{
    public:
     ContoursProcessorClass() ;
    ~ContoursProcessorClass() {};

    void SetImage(const cv::Mat& Image);
    void operator()(const cv::Mat& Image){ SetImage(Image);}

                      int ContoursCount = 0;
      std::vector<double> ContourAreas;
    std::vector<cv::Rect> ContourRects;
    std::vector<cv::Rect> ContourRectsFiltered;

    std::vector<cv::Rect> ContourRectsSorted;
                cv::Rect getMaxRect();
                   cv::Rect getRect(int num);
                cv::Rect rectMax;

                int countContours();
                int countContoursFiltered();
     std::pair<float,float> getPosMaxRect();
     std::pair<float,float> getPosRect(int num);
     bool isAvailable(int num) { return num < ContoursCount; } 

    std::vector<cv::Rect>::iterator begin() { return ContourRects.begin(); };
    std::vector<cv::Rect>::iterator end()   { return RectEnd; };

    std::vector<cv::Rect>::iterator beginFiltered() { return ContourRectsFiltered.begin(); };
    std::vector<cv::Rect>::iterator endFiltered()   { return RectEndFiltered; };

        void PrintContoursRect(cv::Mat& OutputImage);

    private:
        cv::Mat InputImage;
        int minLimit = 20*20;
        int maxLimit = 60*60;

    std::vector<cv::Rect>::iterator RectOutput;
    std::vector<cv::Rect>::iterator RectEnd;

    std::vector<cv::Rect>::iterator RectOutputFiltered;
    std::vector<cv::Rect>::iterator RectEndFiltered;

    std::vector<std::vector<cv::Point>> Contours;
                std::vector<cv::Vec4i>  Hierarchy;

};

#endif
