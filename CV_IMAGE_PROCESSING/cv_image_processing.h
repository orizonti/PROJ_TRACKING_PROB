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
#include "engine_statistics.h"
#include "optimization_threshold.h"


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

class ContoursProcessorClass
{
    public:
    ContoursProcessorClass() ;
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

    QPair<double,double> GetCentroid(const cv::Mat& Image); 
    int NumberMaxContour = 0;

    std::vector<std::vector<cv::Point>> Contours;
    std::vector<std::vector<cv::Point>> ContoursExtern;

    std::vector<cv::Vec4i>  Hierarchy;

    std::vector<double> ContourAreas;
    std::vector<cv::Rect> ContourRects;
    std::vector<cv::Rect> ContourRectsOld;
    std::vector<Statistic> ContourStatistics;

    void operator()(const cv::Mat& Image){ SetImage(Image);}

	bool isROIValid(const cv::Rect& ROI, int SizeImage);
	void CheckROI(cv::Rect& ROI, int SizeImage);
    int DelayCounter = 0;
};

class FilterBlotchClass
{
    public:
    void FilterImage(cv::Mat& FilteringImage, cv::Mat& RawImage);
    ContoursProcessorClass FindContours;
    QString TAG_NAME{"[ BLOTCH_FILTER ]"};
};

struct TrackingDataStruct
{
    std::pair<float,float> Position;
    std::pair<float,float> Velocity;
    cv::Rect RectObject;
};


class ModuleImageProcessing : public ImageSourceInterface, public PassTwoCoordClass
{
Q_OBJECT
public:
ModuleImageProcessing(QObject* parent = 0) : ImageSourceInterface(parent) {} 

    //==================================================
    void SetProcessingRegim(int Regim)      ;
    void StartStopProcessing(bool StartStop);
    void SetThreshold(int Threshold)        ;

                           void SetInput (const QPair<double,double>& Coord){};
    const QPair<double,double>& GetOutput() { return CoordsObject[0]; }

    //==================================================
    QImage&  GetImageToDisplay();
    cv::Mat& GetImageToProcess();

    void GetImageToDisplay(QImage& ImageDst);
    void GetImageToProcess(cv::Mat& ImageDst);

    const std::vector<QPair<int,int>>& GetPoints();  
    const std::vector<QRect>&          GetRects();  
    const QString&                     GetInfo();  
    //==================================================

    void LinkToModule(std::shared_ptr<ImageSourceInterface> ImageSource);
    friend std::shared_ptr<ModuleImageProcessing> operator| (std::shared_ptr<ImageSourceInterface > Source, 
                                                             std::shared_ptr<ModuleImageProcessing> Rec);
    public slots:
    virtual void SlotProcessImage(const cv::Mat& Image) = 0;
    virtual void SlotProcessImage() = 0;

    //==================================================
    bool IsROIValid(cv::Rect& ROI);
    void CheckCorrectROI(cv::Rect& ROI);
    virtual void CalcThreshold() {};
    //=======================================
    protected:
    std::mutex MutexImageAccess;
    std::queue<cv::Mat> ImageProcStore;
    cv::Mat  ImageInput;
    cv::Mat  ImageInputTemp;
    cv::Mat  ImageProcessing;
    cv::Mat  ImageProcessingROI;
    QImage   ImageToDisplay;

    //===================================================
    std::shared_ptr<ImageSourceInterface> SourceImage;
    std::shared_ptr<PassTwoCoordClass   > ReceiverLinkCoord;
    //===================================================
    std::vector<QPair<double,double>> CoordsObject{2};
    std::vector<cv::Rect>  RectsObject{2};
    std::vector<cv::Point> PointsProcess{2};

    std::vector<QPair<int,int>> CoordsImage{2};
    std::vector<QRect>          RectsImage{2};

    int Threshold = 50;
    //==================================================
    QTimer timerProcessImage;
    QTimer timerDisplay;
    std::chrono::time_point<std::chrono::high_resolution_clock> ProcessTimePoint;

    public:
	std::string TAG_NAME{"[ TRACKER_CENTROID ]"};
    QString ProcessInfo = "[ NO DATA ]";

};

class ImageTrackerCentroid : public ModuleImageProcessing
{
    Q_OBJECT
public:
    explicit ImageTrackerCentroid(QObject* parent = 0);
    ~ImageTrackerCentroid();
    int NumberChannel = 0;
    static int NumberChannels;

    ContoursProcessorClass ContoursProcessor;
        FilterBlotchClass FilterBlotch;
    ThresholdOptimizatorEngine ThresholdAdjuster{PROCESS_METHOD::PARALLEL_BY_DISPERSION};
    
    //Statistic StatisticCoord{40};
    //Statistic StatisticDispersion{120};
    //Statistic StatisticThreshold{200};

    Statistic StatisticCoord{100};
    Statistic StatisticDispersion{100};
    Statistic StatisticThreshold{100};

    ValueDetector<double> TrackingDetector;
    ValueSaturation<double> Saturation;
    ValueBinaryInversion<double> InversionBinary;
    
    void FindObjectCentroid(cv::Mat& Image);
    void TrackObjectCentroid(cv::Mat& Image);
    bool FLAG_OBJECT_FOUND = false;

    void CalcCentroid(cv::Mat& Image);
    void CalcThreshold() override;
    QPair<double,double> GetCentroid(cv::Mat& Image);

    std::function<void (cv::Mat&, cv::Mat&)> FilterSharpen;
    std::function<void (cv::Mat&, cv::Mat&)> FilterSobel;
    std::function<void (cv::Mat&, cv::Mat&)> FilterErosion;

public  slots:
   void SlotProcessImage(const cv::Mat& Image) override;
   void SlotProcessImage() override;
};

class ImageTrackerTemplate : public ImageTrackerCentroid
{
    Q_OBJECT
public:
    explicit ImageTrackerTemplate(QObject* parent = 0);
    ~ImageTrackerTemplate();

    ContoursProcessorClass ContoursProcessor;
        FilterBlotchClass FilterBlotch;

    cv::Mat  ImageTemplate;
    cv::Mat  ProcessResult;
    cv::Size TemplateRect;

    QPair<double,double> VelAimAvarage = QPair<double,double>(0,0); 
    std::vector<QPair<int,int>> Trajectory;
    std::vector<QPair<int,int>> TrajectoryIncrement;

    bool  FLAG_FAST_MATCHING = false;

    void FindImageTemplate(cv::Mat& Image);
    void TrackImageTemplate(cv::Mat& Image);

    void CalcObjectTrajectory();

public  slots:
   void SlotProcessImage(const cv::Mat& Image) override;
   void SlotProcessImage() override;
};


#endif // CV_IMAGE_PROC_H
