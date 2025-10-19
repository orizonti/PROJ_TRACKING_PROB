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
    virtual QImage&  getImageToDisplay() = 0;
    virtual cv::Mat& getImageToProcess() = 0;

    virtual void getImageToDisplay(QImage& ImageDst) = 0;
    virtual void getImageToProcess(cv::Mat& ImageDst) = 0;

    virtual const std::vector<QPair<int,int>>& getPoints() = 0;  
    virtual const std::vector<QRect>& getRects() = 0;  
    virtual const QString& getInfo() = 0;  

    virtual bool switchToNextFrame() { return false; };
    virtual void skipFrames() {};

    virtual int getAvailableFrames() { return 0;};
    virtual bool isFrameAvailable()  { return getAvailableFrames() > 0;};

    virtual std::pair<int,int>     getImageSize()  { return std::pair<int,int>(720,540);};
    virtual               int      getImageDepth() { return CV_8UC1;};
    virtual std::pair<float,float> getFramePeriod(){ return std::pair<float,float>(0,0);};  //PERIOD GET, PERIOD PROCESS

    virtual std::shared_ptr<ImageSourceInterface> getImageSourceChannel() {return std::shared_ptr<ImageSourceInterface>(this);};

    void adjustImageReceiver(cv::Mat& receiver);
    
    FramePeriodMeasure FrameMeasureInput;
    FramePeriodMeasure FrameMeasureProcess;

    signals:
    void signalNewImage(const cv::Mat&);
    void signalNewImage(const QImage&);
    void signalNewImage();

};