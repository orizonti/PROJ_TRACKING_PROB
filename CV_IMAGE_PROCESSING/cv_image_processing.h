#ifndef CV_IMAGE_PROC_H
#define CV_IMAGE_PROC_H

//#include <chrono>
#include <QObject>
#include <QDebug>

#include <QTimer>
#include <QImage>
#include <QString>
#include <memory>
#include <opencv2/opencv.hpp>
#include <QFile> 
#include <chrono>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <tuple>
#include <mutex>
#include "interface_pass_coord.h"
#include "interface_image_source.h"

bool operator==(std::pair<int,int> pair, std::pair<int,int> pair2);
bool operator!=(std::pair<int,int> pair, std::pair<int,int> pair2);
bool operator==(std::pair<int,int> pair, int value);
bool operator!=(std::pair<int,int> pair, int value);
template<typename T> 
cv::Rect& operator*(T Scale, cv::Rect& rect)
{
return rect*Scale;
}

template<typename T> 
cv::Rect& operator*(cv::Rect& rect, T Scale)
{
	rect.x -= (rect.width*Scale - rect.width)/2;
	rect.y -= (rect.height*Scale - rect.height)/2;
	rect.width *= Scale;
	rect.height *= Scale;
	return rect;
}

class FindContoursProcessor
{
    public:
    void SetImage(const cv::Mat& Image);
    void PrintContoursRect(cv::Mat& OutputImage);
    void ClearBlotchRects(cv::Mat& OutputImage);
    cv::Rect GetMaxContourRect();
    int NumberMaxContour = 0;

    std::vector<std::vector<cv::Point>> Contours;
    std::vector<std::vector<cv::Point>> ContoursExtern;

    std::vector<cv::Vec4i>  Hierarchy;

    std::vector<double> ContourAreas;
    std::vector<cv::Rect> ContourRects;
    void operator()(const cv::Mat& Image){ SetImage(Image);}

	bool isROIValid(const cv::Rect& ROI, int SizeImage);
	void CheckROI(cv::Rect& ROI, int SizeImage);
};

class FilterBlotchClass
{
    public:
    void FilterImage(cv::Mat& FilteringImage, cv::Mat& RawImage);
    FindContoursProcessor FindContours;
    QString TAG_NAME{"[ BLOTCH_FILTER ]"};
};

struct TrackingDataStruct
{
    std::pair<float,float> Position;
    std::pair<float,float> Velocity;
    cv::Rect RectObject;
};


class WindowImageProcessingControlInterface
{
public:
virtual void SetProcessingRegim(int Regim) = 0;
virtual void StartStopProcessing(bool StartStop) = 0;
virtual void SetThreshold(int Threshold) = 0;
};

class CVImageProcessing : public ImageSourceInterface, public WindowImageProcessingControlInterface, public PassTwoCoordClass
{
    Q_OBJECT
public:
    explicit CVImageProcessing(QObject* parent = 0);
    ~CVImageProcessing();

	std::string TAG_NAME{"[ IMAGE_PROC ]"};
    QString ProcessInfo = "[ NO DATA ]";

    std::mutex MutexImageAccess;

    //==================================================
    std::queue<cv::Mat> ImageProcStore;
    cv::Mat  ImageInput;
    cv::Mat  ImageProcessing;
    cv::Mat  ImageTemplate;
    cv::Mat  ProcessResult;
    cv::Size TemplateRect;
    //==================================================
    QImage   ImageToDisplay;
    //==================================================

    std::vector<QPair<int,int>> Trajectory;
    std::vector<QPair<int,int>> TrajectoryIncrement;
    std::vector<QPair<int,int>> TrajectoryIncrementAvarage;

    QPair<double,double> PosAim;
               cv::Point PosAimProcess;
                cv::Rect ROIAim;
    QPair<double,double> VelAimAvarage = QPair<double,double>(0,0); 
    std::chrono::time_point<std::chrono::high_resolution_clock> ProcessTimePoint;
    //==================================================

    std::atomic<bool> FLAG_DISPLAY_IMAGE = false;
                bool  FLAG_FAST_MATCHING = false;
    //==================================================

    FindContoursProcessor ContoursProcessor;
        FilterBlotchClass FilterBlotch;
    
    int Counter = 0;
    int DispCounter = 0;
    int Threshold = 50;
    int WorkRegim = 1;

    int GetID(); 
    //===================================================
    // ImageSourceInterface
    QImage& GetImageToDisplay();
    cv::Mat& GetImageToProcess();

    void GetImageToDisplay(QImage& ImageDst);
    void GetImageToProcess(cv::Mat& ImageDst);

    const std::vector<QPair<int,int>>& GetPoints();  
    const std::vector<QRect>&          GetRects();  
    const QString&                     GetInfo();  
    //==================================================
    std::vector<QPair<int,int>> ImagePoints{2};
    std::vector<QRect>          ImageRects{2};
    //===================================================
    std::shared_ptr<ImageSourceInterface> SourceImage;
    std::shared_ptr<PassTwoCoordClass> ReceiverLinkCoord;

    void SetInput(const QPair<double,double>& Coord){};
    const QPair<double,double>& GetOutput() { return PosAim; }

    //=======================================
    void SetProcessingRegim(int Regim);
    void StartStopProcessing(bool StartStop);
    void SetThreshold(int Thresh);
    //=======================================

    bool IsROIValid(cv::Rect& ROI);
    void CheckCorrectROI(cv::Rect& ROI);

    void LinkToModule(std::shared_ptr<ImageSourceInterface> ImageSource);

    friend std::shared_ptr<CVImageProcessing> operator|(std::shared_ptr<ImageSourceInterface> Source, 
                                                        std::shared_ptr<CVImageProcessing> Rec);
    void CalcVelocity();

    void FindImageTemplate(cv::Mat& Image);
    void FindImageCentroid(cv::Mat& Image);
    QTimer timerProcessImage;
    QTimer timerDisplay;


public  slots:
   void SlotProcessImage(const cv::Mat& Image);
   void SlotProcessImage();
   void SlotDisplayProcessingImage();

};

#endif // CV_IMAGE_PROC_H
