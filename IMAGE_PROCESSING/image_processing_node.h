#ifndef IMAGE_PROC_NODE_H
#define IMAGE_PROC_NODE_H

#include <QObject>
#include <QDebug>

#include <QTimer>
#include <QImage>
#include <QString>
#include <memory>
#include <opencv2/opencv.hpp>
#include <QFile> 
#include <mutex>
#include "interface_pass_coord.h"
#include "interface_image_source.h"
#include "interface_image_source.h"
#include <debug_output_filter.h>
#include "device_generic_interface.h"
#include "interface_node_signal_adapter.h"
#include "register_settings.h"
#include "thread_operation_nodes.h"


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
          enum class ModesModule  { Master = 0, SlaveActive = 1, SlavePassive = 2};
          enum class StatesModule { Disabled = 0, Idle = 1, WorkTrack = 2};

ModuleImageProcessing(QString name = "[TRACKER]", QObject* parent = 0); 
ModuleImageProcessing(int width, int height, int size,QString name = "[TRACKER]" , QObject* parent = 0); 

~ModuleImageProcessing() { } 

    //==================================================
                   int periodProcess = 20;
                   QTimer timerProcessImage;
                   void printInfo();
    public slots:
    virtual void SlotProcessImage(const cv::Mat& Image) {};
    virtual void SlotProcessImage() = 0;
    friend ModuleImageProcessing& operator>>(const cv::Mat& Image, ModuleImageProcessing& Module);
    friend std::shared_ptr<ModuleImageProcessing> operator>>(const cv::Mat& Image, std::shared_ptr<ModuleImageProcessing> Module);
    
    public:

    NodeCoordPassNop<float> NopNode;
    PassCoordClass<float>& operator>>(PassCoordClass<float>& Receiver) override 
    { 
    if(PassCoordClass<float>::PassBlocked) return NopNode; 
              CoordsObject[0] >> Receiver; return Receiver; }


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
        bool isSourceActive() override { return isEnabled(); }
        void getImageToDisplay(QImage& ImageDst)   override;

    const std::vector<QPair<int,int>>& getPoints() override;  
    const std::vector<QRect>&           getRects() override;  
    const std::string&                   getInfo() override;  
                                 QString getName() override { return "[PROCESSING NODE]"; };
            std::pair<float,float> getTickPeriod() override ;
    //===================================================
                      std::shared_ptr<SourceImageInterface  > SourceImage    = nullptr;
    
    void linkToModule(std::shared_ptr<SourceImageInterface > ImageSource);
    void linkToModule(std::shared_ptr<ModuleImageProcessing> ImageSource);
    void printLinks();

    friend std::shared_ptr<ModuleImageProcessing> operator|(std::shared_ptr<SourceImageInterface > Source, 
                                                            std::shared_ptr<ModuleImageProcessing> Rec);

    friend std::shared_ptr<ModuleImageProcessing> operator|(std::shared_ptr<ModuleImageProcessing> Source, 
                                                            std::shared_ptr<ModuleImageProcessing> Dst);

    bool isLinkedSource() { return SourceImage != nullptr;}

    std::vector<std::shared_ptr<ModuleImageProcessing>> Links;
    //===================================================================================
                          void setInput(const QPair<float,float>& Coord) override;
    const QPair<float,float>& getOutput()           override  { return CoordsObject[0]; }
    
    void SetSlaveMode(ModesModule Mode)   { ModeProcessing  = Mode; StateProcessing = StatesModule::Idle; }

    void SetStateWork    ();
    void SetStateIdle    ();
    void SetStateDisabled(); 
    void SetBlockOutput  (bool OnOff);

    bool isDisabled()  { return StateProcessing == StatesModule::Disabled; } 
    bool isIdle()      { return StateProcessing == StatesModule::Idle;     } 
    virtual bool isTrackMiss() { return StateProcessing != StatesModule::WorkTrack;}
    virtual bool isTrackHold() { return StateProcessing == StatesModule::WorkTrack;}

    bool isModuleMaster()  { return ModeProcessing == ModesModule::Master;      };
    bool isModuleSlave()   { return ModeProcessing == ModesModule::SlaveActive; };
    bool isModulePassive() { return ModeProcessing == ModesModule::SlavePassive;};
    //===================================================================================
    virtual void resetState();
    public slots:
    virtual void SlotSetInput(const QPair<float,float>& Coord) { setInput(Coord); };

    virtual void SlotResetProcessing();
    virtual void SlotStopProcessing ();
    virtual void SlotStartProcessing();

    virtual void SlotSelectObject(std::pair<float,float> PointRelative) {};
            void SlotBlockOutput(bool OnOff) { this->PassBlocked = OnOff; } ;
    //===================================================================================
    public:
    void resetOutput();

    virtual void SetThreshold(float Value)    { this->Threshold = Value; } ;
            void SetPeriodProcess(int period) { periodProcess = period; };
    //===================================================
    //DeviceGenericHandleControl
	  void setParam (uint16_t CommandID, float    CommandParam) override {};
	  void setValue (float Value) override { SetThreshold((int)Value); };
	  void setEnable(bool OnOff, uint16_t Number = 0) override;
    bool isEnabled()    override { return !isDisabled(); } 

    bool isTypeActive() override { return true; } 



    NodeSignalAdapter NodeSignalEnable{this,0};
    NodeSignalAdapter NodeSignalFault {this,1};
    //===================================================

    protected:
    std::mutex MutexImageAccess;
    std::mutex MutexImageAccessDisplay;
    std::mutex MutexInput;

    std::vector<cv::Mat> ImagesInput{10};
    std::vector<cv::Mat> ImagesOutput{10};

    std::vector<cv::Mat>::iterator ImageInput {ImagesInput.begin()};
    std::vector<cv::Mat>::iterator ImageOutput{ImagesOutput.begin()};

    cv::Mat  ImageTemp1;
    cv::Mat  ImageTemp2;
    cv::Mat  ImageTemp3;
    cv::Mat  ImageProcessing;
    cv::Mat  ImageProcessingROI;
      QImage ImageToDisplay;

    //==================================================
            bool IsROIValid(cv::Rect& ROI);
            void CheckCorrectROI(cv::Rect& ROI);
    //===================================================

    public:
    //PROCESSING DATA
    std::vector<QPair<float,float>> CoordsObject {3};
             std::vector<cv::Rect > RectsObject  {2};
             std::vector<cv::Point> PointsProcess{2};
                                int Threshold = 50;
                                int SizeROI  = SettingsRegister::GetValue("PROCESSING_ROI1");
                 QPair<float,float> SizeImage{SettingsRegister::GetPair("CAMERA_SIZE_ACTIVE")};

               StatesModule StateProcessing { StatesModule::Disabled }; 
                ModesModule ModeProcessing  { ModesModule::Master}; 
    //=======================================================================================
    public:
    //DISPLAY DATA
    std::string TAG_NAME = "[ TRACKER ]";
    std::string INFO     = "[ NO DATA ]";
    std::vector<QPair<int,int>> CoordsImage{2};
    std::vector<QRect>          RectsImage {2};
    std::vector<double>         ValuesImage{0,0,0,0,0,0,0,0,0,0,0};
    //=======================================================================================
    signals:
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
