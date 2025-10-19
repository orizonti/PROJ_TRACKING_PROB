#ifndef CONTOUR_PROCESSING_H
#define CONTOUR_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <QPair>
#include "engine_statistics.h"

class ContoursProcessorClass
{
    public:
    ContoursProcessorClass() ;
    ~ContoursProcessorClass() {};
    void SetImage(const cv::Mat& Image);
    void PrintContoursRect(cv::Mat& OutputImage);
    void ClearBlotchRects(cv::Mat& OutputImage);
    void CalcContoursStatistic();
    cv::Mat InputImage;
    cv::Mat MaskImage;
    cv::Rect ObjectRect{100,100,300,300};

    cv::Rect GetMaxContourRect();
    cv::Rect GetMostStableContour();
    double GetMinimumContourDispersion();

    QPair<float,float> GetCentroid(const cv::Mat& Image); 
    int NumberMaxContour = 0;

    std::vector<std::vector<cv::Point>> Contours;
    std::vector<std::vector<cv::Point>> ContoursExtern;

    std::vector<cv::Vec4i>  Hierarchy;

    std::vector<double> ContourAreas;
    std::vector<cv::Rect> ContourRects;
    std::vector<cv::Rect> ContourRectsOld;
    std::vector<StatisticNode<double>> ContourStatistics;

    void operator()(const cv::Mat& Image){ SetImage(Image);}

	bool isROIValid(const cv::Rect& ROI, int SizeImage);
	void CheckROI(cv::Rect& ROI, int SizeImage);
    int DelayCounter = 0;
};

#endif
