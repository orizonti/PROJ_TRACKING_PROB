#pragma once

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QString>

class FramePeriodMeasure
{
    public:
    void PushTick()
    {

    if(counter >= 1)  { TimePoint =   std::chrono::high_resolution_clock::now();
                        Duration  = TimePoint - TimePointLast; 
                        TimePointLast = TimePoint;
                        FramePeriod   = Duration.count()*1000;
                        FrameFrequency = 1/Duration.count(); 
                        counter = 0;
                        return;
                        }

     TimePointLast = std::chrono::high_resolution_clock::now(); counter++; 

    }
    void operator++(int) { PushTick();}
    float FramePeriod{0};
    float FrameFrequency{0};
    int counter = 0;

    QString printPeriod()
    {
      return QString("[ PROCESS PERIOD ] %1").arg(FramePeriod*1000); 
    };

    private:
    std::chrono::time_point<std::chrono::high_resolution_clock> TimePointLast;
    std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
    std::chrono::duration<double> Duration;
};

class ImageSourceInterface: public QObject
{
    Q_OBJECT
    public:
    ImageSourceInterface(QObject* parent = 0);
    virtual QImage&  GetImageToDisplay() = 0;
    virtual cv::Mat& GetImageToProcess() = 0;

    virtual void GetImageToDisplay(QImage& ImageDst) = 0;
    virtual void GetImageToProcess(cv::Mat& ImageDst) = 0;

    virtual const std::vector<QPair<int,int>>& GetPoints() = 0;  
    virtual const std::vector<QRect>& GetRects() = 0;  
    virtual const QString& GetInfo() = 0;  

    virtual bool SwitchToNextFrame() {return false; };
    virtual void SkipFrames() {};

    virtual int GetAvailableFrames() { return 0;};
    virtual bool isFrameAvailable() { return GetAvailableFrames() > 0;};

    virtual std::pair<int,int> GetImageSize() { return std::pair<int,int>(720,540);};
    virtual std::pair<float,float> GetFramePeriod() { return std::pair<float,float>(0,0);};  //PERIOD GET, PERIOD PROCESS

    virtual std::shared_ptr<ImageSourceInterface> GetImageSourceChannel() {return std::shared_ptr<ImageSourceInterface>(this);};
    
    FramePeriodMeasure FrameMeasureInput;
    FramePeriodMeasure FrameMeasureProcess;

    signals:
    void SignalNewImage(const cv::Mat&);
    void SignalNewImage(const QImage&);
    void SignalNewImage();

};