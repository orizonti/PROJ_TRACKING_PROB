#ifndef CV_IMAGE_PROC_H
#define CV_IMAGE_PROC_H

//#include <chrono>
#include <QObject>
#include <QDebug>

#include <QTimer>
#include <QImage>
#include <QString>
#include <opencv2/opencv.hpp>
#include <QFile>
#include <chrono>
#include "AIM_IMAGE_IMITATION/AimImageImitatorClass.h"
#include <atomic>
#include <algorithm>
#include <chrono>

bool operator==(std::pair<int,int> pair, std::pair<int,int> pair2);
bool operator!=(std::pair<int,int> pair, std::pair<int,int> pair2);
bool operator==(std::pair<int,int> pair, int value);
bool operator!=(std::pair<int,int> pair, int value);

class FindContoursProcessor
{
    public:
    void SetImage(const cv::Mat& Image);
    void PrintContoursRect(cv::Mat& OutputImage);
    void ClearBlotchRects(cv::Mat& OutputImage);
    int NumberMaxContour = 0;

    std::vector<std::vector<cv::Point>> Contours;
    std::vector<std::vector<cv::Point>> ContoursExtern;

    std::vector<cv::Vec4i>  Hierarchy;

    std::vector<double> ContourAreas;
    std::vector<cv::Rect> ContourRects;
    void operator()(const cv::Mat& Image){ SetImage(Image);}

	bool isROIValid(const cv::Rect& ROI, int ImageSize);
	void CheckROI(cv::Rect& ROI, int ImageSize);
};

class FilterBlotchClass
{
    public:
    void FilterImage(cv::Mat& FilteringImage, cv::Mat& RawImage);
    FindContoursProcessor FindContours;
    QString TAG{"[ BLOTCH_FILTER ]"};
};

class CVImageProcessing : public ImageSourceInterface
{
    Q_OBJECT
public:
    explicit CVImageProcessing(QObject* parent = 0);
    ~CVImageProcessing();
    QString MODULE_NAME = "CV_IMAGE_PROCESSING";
    QImage LastImageToDisplay;
    std::queue<cv::Mat> ImageProcStore;
    cv::Mat ImageInput;
    cv::Mat ImageProcessing;
    cv::Mat ImageTemplate;
    cv::Mat ProcessResult;
    cv::Size TemplateRect;
    std::chrono::time_point<std::chrono::high_resolution_clock> ProcessTimePoint;

    std::vector<std::pair<int,int>> Trajectory;
    std::vector<std::pair<int,int>> TrajectoryIncrement;
    std::vector<std::pair<int,int>> TrajectoryIncrementAvarage;

    std::pair<float,float> centroid_abs;
    cv::Point MatchObjectPos;
    cv::Point MatchObjectPosROI;
    std::pair<float,float> AvarageVel = std::pair<int,int>(0,0); 
    cv::Rect ROIFindRect;
    QImage ImageToDisplay;
    std::atomic<bool> FLAG_DISPLAY_IMAGE = false;

    bool FLAG_FAST_MATCHING = false;

    FindContoursProcessor ContoursProcessor;
    FilterBlotchClass FilterBlotch;
    
    int Counter = 0;
    int DispCounter = 0;

    bool IsROIValid(cv::Rect& ROI);
    void CheckCorrectROI(cv::Rect& ROI);

    void LinkToImageSource(ImageSourceInterface* ImageSource);
    void CalcVelocity();

    void FindImageTemplate(cv::Mat& Image);
    void FindImageCentroid(cv::Mat& Image);
    QTimer timerProcessImage;
    QTimer timerDisplay;

    QImage   GetNewImageDisplay();
    cv::Mat& GetNewImageProcess();

   void SlotProcessImage();
public  slots:
   void SlotSetImageToProcess(const cv::Mat& Image);
   void SlotDisplayProcessingImage();
signals:
   void SignalObjectDetected(cv::Point, std::pair<float,float>,cv::Rect);

   void SignalNewImage(const cv::Mat&);
   void SignalNewImage(QImage);
};

#endif // CV_IMAGE_PROC_H
