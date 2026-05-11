#ifndef CONTOUR_PROCESSING_H
#define CONTOUR_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <QPair>
#include <thread>

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
                cv::Rect rectMax;
                cv::Rect rectMaxScaled;
                cv::Rect rectObject;
                cv::Rect rectImage{1,1,100,100};

    std::vector<cv::Rect> rectsMaxOutside{4};
    std::vector<cv::Rect>& getMaxOutsideRects();

                   cv::Rect getMaxRect();
     std::pair<float,float> getPosMaxRect();

     void CheckCorrectROI(cv::Rect& ROI);

                int countContours();
     bool isAvailable(int num) { return num < ContoursCount; } 

    std::vector<cv::Rect>::iterator begin() { return ContourRects.begin(); };
    std::vector<cv::Rect>::iterator end()   { return RectEnd; };

    cv::Rect& getScaledMaxRect(float Scale);

        void PrintContoursRect(cv::Mat& OutputImage);

    private:
        cv::Mat InputImage;
        int minLimit = 3*3;
        int maxLimit = 60*60;

    std::vector<cv::Rect>::iterator RectOutput;
    std::vector<cv::Rect>::iterator RectEnd;

    std::vector<cv::Rect>::iterator RectOutputFiltered;
    std::vector<cv::Rect>::iterator RectEndFiltered;

    std::vector<std::vector<cv::Point>> Contours;
                std::vector<cv::Vec4i>  Hierarchy;

};

#endif
