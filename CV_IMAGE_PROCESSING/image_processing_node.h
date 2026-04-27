#ifndef IMAGE_PROC_NODE_H
#define IMAGE_PROC_NODE_H

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
#include "debug_output_filter.h"
#include "interface_pass_coord.h"
#include "interface_image_source.h"
#include "engine_statistics.h"
#include "optimization_threshold.h"
#include "./TRACKER_ROBUST/Tracker.h"
#include "state_block_enum.h"
#include "interface_image_source.h"
#include <debug_output_filter.h>


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


class ModuleImageProcessing : public QObject, public SourceImageInterface, public SourceImageDisplayInterface, public PassCoordClass<float>
{
Q_OBJECT
public:
ModuleImageProcessing(QObject* parent = 0); 
~ModuleImageProcessing() { } 

    QString getName() override { return "[PROCESSING NODE]"; };
    //==================================================
    public slots:
    virtual void SlotProcessImage(const cv::Mat& Image) = 0;
    virtual void SlotProcessImage() = 0;
    virtual void SlotResetProcessing();
    virtual void SlotStopProcessing();
    virtual void SlotSetAimPoint(std::pair<float,float> PointRelative) {};
            void SlotCheckSlaveInputPeriod();
            void SlotInputCoord(std::pair<float,float> Coord) { Coord >> *this; };
    
    public:
    virtual bool isROIMode() { return FLAG_TRACK_MODE;};
    virtual void SetProcessingRegim(int Regim);

     int getAvailableFrames() override { return 0; };

    void SetModuleEnabled(bool StartStop);

    void SetThreshold(int Threshold);
    void SetHighFrequencyProcessing() { timerProcessImage.setInterval(2);};
    void SetLowFrequencyProcessing()  { timerProcessImage.setInterval(15);};


    std::shared_ptr<SourceImageInterface> getImageSourceChannel() override { return std::shared_ptr<SourceImageInterface>(this);};

    void setInput (const QPair<float,float>& Coord) override;
    const QPair<float,float>& getOutput()           override  { return CoordsObject[0]; }

           int NumberModule{0};
    static int CountModules;
           int Counter = 0;


    bool FLAG_OBJECT_HOLD = false;
    bool FLAG_TRACK_MODE  = false;
    bool FLAG_SLAVE_MODE  = false;
    bool FLAG_CHECK_OBJECT_HOLD = false;

    MeasurePeriodNode MeasurePeriodMasterInput;
    MeasurePeriodNode FrameMeasureInput;
    MeasurePeriodNode FrameMeasureProcess;
    QTimer timerCheckSlaveInput;
    

    static std::vector<ModuleImageProcessing*> Modules;
    //==================================================
    void getImageToDisplay(QImage& ImageDst)  override;

    cv::Mat& getImageToProcess()              override;
    void getImageToProcess(cv::Mat& ImageDst) override;
    std::pair<int,int> getSizeImage() override { return std::make_pair(ImageProcessing.cols, ImageProcessing.rows); }

    const std::vector<QPair<int,int>>& getPoints()  override;  
    const std::vector<QRect>&          getRects()   override;  
    const QString&                     getInfo()    override;  

    //==================================================
    
    //int getNumberSource() override { return NumberModule; };

    void linkToModule(std::shared_ptr<SourceImageInterface > ImageSource);
    void linkToModule(std::shared_ptr<ModuleImageProcessing> ImageSource);
    void setSlaveMode() { FLAG_SLAVE_MODE = true; FLAG_TRACK_MODE = true; timerCheckSlaveInput.start(); }  

    friend std::shared_ptr<ModuleImageProcessing> operator| (std::shared_ptr<SourceImageInterface > Source, 
                                                             std::shared_ptr<ModuleImageProcessing> Rec);

    friend void operator | (std::shared_ptr<ModuleImageProcessing > Source, std::shared_ptr<ModuleImageProcessing> Dst);


    void SetImageParam(int MinWeight, int MaxWeight, int Distortion);
    void SetObjectHoldCheck(bool Flag) { FLAG_CHECK_OBJECT_HOLD = Flag;};


    //==================================================
            bool IsROIValid(cv::Rect& ROI);
            void CheckCorrectROI(cv::Rect& ROI);
    virtual void CalcThreshold() {};
    //=======================================
    bool isLinked() { return SourceImage != nullptr;}
    bool isLinkedSlave() { return SlaveProcessor != nullptr;}

    protected:
    static std::mutex MutexImageAccess;
    std::queue<cv::Mat> ImageProcStore;
    cv::Mat  ImageInput;
    cv::Mat  ImageInputTemp;
    cv::Mat  ImageProcessing;
    cv::Mat  ImageProcessingROI;

    cv::UMat ImageGPU;
    cv::UMat ImageGPUTemp;
    cv::UMat ImageGPURoi;
      QImage ImageToDisplay;

    //===================================================
    std::shared_ptr<SourceImageInterface> SourceImage = nullptr;
    std::shared_ptr<ModuleImageProcessing> SlaveProcessor = nullptr;
    std::shared_ptr<PassCoordClass<float>> ReceiverLinkCoord;
    //===================================================

    std::pair<float,float> getTickPeriod() override ;
    //===================================================

    std::vector<QPair<float,float>> CoordsObject{3};
    std::vector<cv::Rect>  RectsObject{2};
    std::vector<cv::Point> PointsProcess{2};

    std::vector<QPair<int,int>> CoordsImage{2};
    std::vector<QRect>          RectsImage{2};
    std::vector<double>         ValuesImage{0,0,0,0,0,0,0,0,0,0,0};

    float ThresholdCorrection = 0.7;
      int Threshold = 50;
      int ROI_SIZE = 60;
      int MaxImageWeight = 150*150*255;
      int MinImageWeight = 4*4*255;
      int MaxImageDistortion = 8;
    //==================================================
    std::chrono::time_point<std::chrono::high_resolution_clock> ProcessTimePoint;
    std::chrono::time_point<std::chrono::high_resolution_clock> ProcessTimePoint2;

    public:
    QString TAG_NAME = QString("[ %1 ] ").arg("TRACKER");
    QString ProcessInfo = "[ NO DATA ]";

    QTimer timerProcessImage;
    QTimer timerDisplay;

    signals:
    void SignalSetFrequency(int Frequency);
    void SignalBlockEnabled(bool);
    void SignalTrackCoord(std::pair<float,float> Coord);
};



#endif 
