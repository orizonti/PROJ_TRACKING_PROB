#pragma once

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QString>

class ImageSourceInterface: public QObject
{
    Q_OBJECT
    public:
    ImageSourceInterface(QObject* parent = 0);
    virtual QImage& GetImageToDisplay() = 0;
    virtual cv::Mat& GetImageToProcess() = 0;

    virtual void GetImageToDisplay(QImage& ImageDst) = 0;
    virtual void GetImageToProcess(cv::Mat& ImageDst) = 0;

    virtual const std::vector<QPair<int,int>>& GetPoints() = 0;  
    virtual const std::vector<QRect>& GetRects() = 0;  
    virtual const QString& GetInfo() = 0;  
    
    signals:
    void SignalNewImage(const cv::Mat&);
    void SignalNewImage(const QImage&);
    void SignalNewImage();

};