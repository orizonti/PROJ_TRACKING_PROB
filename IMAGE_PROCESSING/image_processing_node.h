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
#include "device_generic_interface.h"
#include "interface_node_signal_adapter.h"


class NodeRectToCoord: public PassCoordClass<float>
{
  public:
    friend PassCoordClass<float>& operator>>(cv::Rect& rect, NodeRectToCoord& Node)
    {
      Node.OutputCoord.first  = rect.x + rect.width/2;
      Node.OutputCoord.second = rect.y + rect.height/2;
      return Node;
    }
    static std::pair<float,float> Convert(cv::Rect& rect)
    {
      return std::make_pair(rect.x + rect.width/2, rect.y + rect.height/2);
    }

};



class ModuleImageProcessing : public QObject, 
                              public SourceImageInterface, 
                              public SourceImageDisplayInterface, 
                              public PassCoordClass<float>,
                              public DeviceGenericHandleControl
{

Q_OBJECT
public:
          enum class ModesModule  { Master = 0, Slave = 1};
          enum class StatesModule { Idle = 0, WorkSearch = 1, WorkTrack = 2};

ModuleImageProcessing(QString name = "[TRACKER]", QObject* parent = 0); 
ModuleImageProcessing(int width, int height, int size,QString name = "[TRACKER]" , QObject* parent = 0); 

~ModuleImageProcessing() { } 

    //==================================================
                   QTimer timerProcessImage;
    public slots:
    virtual void SlotProcessImage(const cv::Mat& Image) = 0;
    virtual void SlotProcessImage() = 0;
    friend ModuleImageProcessing& operator>>(const cv::Mat& Image, ModuleImageProcessing& Module);
    friend std::shared_ptr<ModuleImageProcessing> operator>>(const cv::Mat& Image, std::shared_ptr<ModuleImageProcessing> Module);
    
    public:

    MeasurePeriodNode FrameMeasureInput;
    MeasurePeriodNode FrameMeasureProcess;
    void moveToThread(QThread* thread);
    //==================================================
    //
    //SourceImageInterface
    virtual void skipFrames() override {};

    cv::Mat& getImageToProcess()                  override;
        void getImageToProcess(cv::Mat& ImageDst) override;
    std::pair<int,int> getSizeImage()             override { return std::make_pair(ImageProcessing.cols, 
                                                                                   ImageProcessing.rows); }
    //===================================================
    //SourceImageDisplayInterface
        void getImageToDisplay(QImage& ImageDst)   override;

    const std::vector<QPair<int,int>>& getPoints() override;  
    const std::vector<QRect>&           getRects() override;  
    const QString&                       getInfo() override;  
                                 QString getName() override { return "[PROCESSING NODE]"; };
            std::pair<float,float> getTickPeriod() override ;
    //===================================================
                      std::shared_ptr<SourceImageInterface  > SourceImage    = nullptr;
    
    void linkToModule(std::shared_ptr<SourceImageInterface > ImageSource);
    void linkToModule(std::shared_ptr<ModuleImageProcessing> ImageSource);

    friend std::shared_ptr<ModuleImageProcessing> operator| (std::shared_ptr<SourceImageInterface > Source, 
                                                             std::shared_ptr<ModuleImageProcessing> Rec);

    friend void operator | (std::shared_ptr<ModuleImageProcessing > Source, std::shared_ptr<ModuleImageProcessing> Dst);

    bool isLinkedSource() { return SourceImage != nullptr;}

    std::vector<std::shared_ptr<ModuleImageProcessing>> Links;
    //===================================================================================
    
               StatesModule StateProcessing { StatesModule::Idle }; 
                ModesModule ModeProcessing  { ModesModule::Master}; 

    void SetSlaveMode()   { ModeProcessing  = ModesModule::Slave; }

    void SetStateActive() { emit signalStart(); };
    void SetStateIdle()   { emit signalStop(); }; 
    void SetReset()       { emit signalReset(); }; 

    bool isIdle()   { return StateProcessing == StatesModule::Idle;  } 
    bool isActive() { return StateProcessing != StatesModule::Idle;  } 
    virtual bool isTrackHold() { return StateProcessing != StatesModule::Idle; }
     
                          void setInput(const QPair<float,float>& Coord) override;
    const QPair<float,float>& getOutput()           override  { return CoordsObject[0]; }
    //===================================================================================
    
    public slots:
    virtual void SlotResetProcessing();
    virtual void SlotStopProcessing ();
    virtual void SlotStartProcessing();

            void SlotBlockOutput(bool OnOff) { this->PassBlocked = OnOff; } ;

    virtual void SlotSelectObject(std::pair<float,float> PointRelative) {};
            void slotInputCoord  (std::pair<float,float> Coord) { setInput(Coord); };

    public:

    void SetThreshold(int Value)      { qDebug() << TAG_NAME << "[THRESHOLD]" << Value; this->Threshold = Value; } ;
    void SetHighFrequencyProcessing() { timerProcessImage.setInterval(1); };
    void SetLowFrequencyProcessing()  { timerProcessImage.setInterval(20);};
    //===================================================
    //DeviceGenericHandleControl
	  void setParam (uint16_t CommandID, float    CommandParam) override {};
	  void setValue (float Value) override { SetThreshold((int)Value); };
	  void setEnable(bool OnOff, uint16_t Number = 0) override;


    NodeSignalAdapter NodeSignalEnable{this,0};
    NodeSignalAdapter NodeSignalFault {this,1};
    //===================================================

    protected:
    std::mutex MutexImageAccess;
    std::mutex MutexInput;

    std::vector<cv::Mat> ImagesInput{10};
    std::vector<cv::Mat>::iterator ImageInput{ImagesInput.begin()};

    cv::Mat  ImageTemp1;
    cv::Mat  ImageTemp2;
    cv::Mat  ImageProcessing;
    cv::Mat  ImageProcessingROI;
    cv::Mat  ImageOutput;
      QImage ImageToDisplay;

    //==================================================
            bool IsROIValid(cv::Rect& ROI);
            void CheckCorrectROI(cv::Rect& ROI);
    //===================================================

    public:
    std::vector<QPair<float,float>> CoordsObject {3};
             std::vector<cv::Rect > RectsObject  {2};
             std::vector<cv::Point> PointsProcess{2};
                                int Threshold = 50;
                                int SizeROI   = 120;

    public:
    QString TAG_NAME = "[ TRACKER ]";
    QString INFO     = "[ NO DATA ]";

    std::vector<QPair<int,int>> CoordsImage{2};
    std::vector<QRect>          RectsImage {2};
    std::vector<double>         ValuesImage{0,0,0,0,0,0,0,0,0,0,0};


    signals:
    void signalCoord(std::pair<float,float> Coord);
    void signalStart();
    void signalStop();
    void signalReset();
};


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

#endif 
