#pragma once

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QString>
#include <QTimer>
#include <QDebug>
#include "module_period_measure.h"

class SourceImageInterface
{
    public:
    virtual cv::Mat& getImageToProcess() = 0;
    virtual     void getImageToProcess(cv::Mat& ImageDst) = 0;

    virtual std::pair<int,int> getSizeImage()       = 0;
    virtual                int getAvailableFrames() { return 0;};
    virtual               bool isFrameAvailable()   { return getAvailableFrames() > 0;};
    virtual std::pair<float,float> getFramePeriod() { return std::pair<float,float>(0,0); }
    virtual std::shared_ptr<SourceImageInterface> getImageSourceChannel() { return std::shared_ptr<SourceImageInterface>(this); }

    virtual bool switchToNextFrame() { return false; };
    virtual void skipFrames() {};
    virtual void adjustImageReceiver(cv::Mat& receiver);

};

class SinkDisplayNode;
class SourceImageDisplayInterface
{
    public:
    virtual     void getImageToDisplay(QImage& ImageDst)  = 0;
    virtual     QImage& getImageToDisplay() = 0;

    virtual const std::vector<QPair<int,int>>& getPoints() = 0;  
    virtual const          std::vector<QRect>& getRects()  = 0;  
    virtual const                     QString& getInfo()   = 0;  
    virtual std::pair<float,float> getFramePeriod() { return std::pair<float,float>(0,0); }

    virtual std::pair<int,int> getSizeImage()  = 0;

    void linkToSink(SinkDisplayNode* Node) { LinkNode = Node;}
    void passToLink();

    private:
    SinkDisplayNode* LinkNode = nullptr;
};

class SinkDisplayNode : public QObject
{
    Q_OBJECT
	public:
    SinkDisplayNode() { connect(&timerSetImage, &QTimer::timeout, this, &SinkDisplayNode::slotGetImage); }

            void setFrequency(int Freq) { Frequency = Freq; timerSetImage.setInterval(1000/Freq); };
	virtual void setImageFrom(SourceImageDisplayInterface* Src) = 0;
    void linkToSource(SourceImageDisplayInterface* Src) { SrcNode = Src; };
    
    SourceImageDisplayInterface* GetSource() { return SrcNode;};
    private:
    SourceImageDisplayInterface* SrcNode = nullptr;

       int Frequency = 30;
    QTimer timerSetImage;
    
    public slots:
    void slotActivate(bool OnOff = true) 
    { 
        if(SrcNode == nullptr) return;
        if(OnOff) timerSetImage.start(1000/Frequency); else timerSetImage.stop(); 
        qDebug() << "ACTIVATE RTSP DISPLAY" << OnOff;
    }; 

    private slots:
	void slotGetImage() { if(SrcNode != nullptr) setImageFrom(SrcNode); };
};

