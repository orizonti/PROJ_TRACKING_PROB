#include "interface_camera_rtsp.h"
#include "interface_image_source.h"
#include <QDebug>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <debug_output_filter.h>
#include <QThread>


using CameraStorageType = CameraImageStorage<CameraInterfaceUniversal>;
std::shared_ptr<SourceImageInterface> CameraInterfaceUniversal::getImageSourceChannel() 
{ 

  auto ChannelStore = ImageStore.back(); 
                      ImageStore.push_back(std::make_shared<CameraStorageType>(this, SizeImage));
  qDebug() << "GET IMAGE SOURCE STORE" << ImageStore.size();
  return ChannelStore;
}

void CameraInterfaceUniversal::slotStartStream() { timerGetFrame.start(10); qDebug() << "[ START RTSP ]"; }
void CameraInterfaceUniversal::slotStopStream()  { timerGetFrame.stop(); }

CameraInterfaceUniversal::CameraInterfaceUniversal(std::string strVideoSource, uint32_t Number, QString NAME) : TAG_NAME(NAME)
{
 
  qDebug() << TAG_NAME << "[CAMERA RESOLUTION]" << SIZE_CAMERA.first << SIZE_CAMERA.second;
  qDebug() << TAG_NAME << "[PROCESS SIZE]"      << SIZE_ROI.first    << SIZE_ROI.second;
  qDebug() << TAG_NAME << "[PROCESS OFFSET]"    << OFFSET_ROI.first  << OFFSET_ROI.second;

  capture.open(strVideoSource, cv::CAP_GSTREAMER);

                           qDebug() << TAG_NAME << "[LINK]" << strVideoSource.c_str();
  if (!capture.isOpened()) qDebug() << "[ERROR] [BAD LINK]" << strVideoSource.c_str();

  QObject::connect(&timerGetFrame, SIGNAL(timeout()),this, SLOT(slotGetFrame()));

  ImageStore.push_back(std::make_shared<CameraStorageType>(this, SizeImage));
}

CameraInterfaceUniversal::~CameraInterfaceUniversal() { qDebug() << "[ THERMAL CAMERA INTERFACE DEINIT]" << TAG_NAME; }


void CameraInterfaceUniversal::getImageToDisplay(QImage& ImageDst) 
{ 
                                 if(CameraStorageType::ImageToDisplay.isNull()) return;
    mutexStorage.lock(); ImageDst = CameraStorageType::ImageToDisplay.copy(); mutexStorage.unlock();
}

cv::Mat& CameraInterfaceUniversal::getImageToProcess()                  { return ImageStore[0]->getImageToProcess(); };
    void CameraInterfaceUniversal::getImageToProcess(cv::Mat& ImageDst) {        ImageStore[0]->getImageToProcess(ImageDst);};

int CameraInterfaceUniversal::getAvailableFrames() { return ImageStore[0]->getAvailableFrames(); }



void CameraInterfaceUniversal::slotGetFrame()
{
        bool isFrameGrabbed = capture.grab();
        if (!isFrameGrabbed) return; 

             isFrameGrabbed = capture.retrieve(inputImage);
        if (!isFrameGrabbed) return; 


        FrameMeasureInput.PushTick();
        FrameMeasureProcess.PushTick();

         //QThread::msleep(100);
                     inputImageCrop = inputImage(rectCrop);
                     inputImageSmall = inputImageCrop;

        //qDebug() << OutputFilter::Filter(10) << "GOT IMAGE " << inputImageSmall.cols << inputImageSmall.rows;
        //cv::resize  (inputImageCrop ,inputImageSmall,cv::Size(SizeImage.first,SizeImage.second));
        cv::cvtColor(inputImageSmall,inputImageGray,cv::COLOR_BGR2GRAY);

        mutexStorage.lock(); 
        CameraStorageType::putNewFrameToStorage(inputImageGray); 
        mutexStorage.unlock();

        FrameMeasureProcess.PushTick();
        //qDebug() << OutputFilter::Filter(10) << "RTSP FRAME PERIOD: " << FrameMeasureInput.TickPeriod*1000 
        //                                     << "PROCESS: " << FrameMeasureProcess.TickPeriod*1000;
}

void CameraInterfaceUniversal::slotEndWork() 
{ 
  qDebug() << TAG_NAME << "[ STOP CAMERA ]";
  slotStopStream(); deinitCamera(); 
  delete this;
}


std::pair<int,int> CameraInterfaceUniversal::getSizeImage() { return ImageStore[0]->getSizeImage(); };

void CameraInterfaceUniversal::moveToThread(QThread* thread)
{
         QObject::moveToThread(thread);
    timerGetFrame.moveToThread(thread);
}

void CameraInterfaceUniversal::enumerateCameras() { }
void CameraInterfaceUniversal::initCamera()       { }
void CameraInterfaceUniversal::deinitCamera()     { }

void CameraInterfaceUniversal::CameraSetSize  (int Width  , int Height) { }
void CameraInterfaceUniversal::CameraSetOffset(int XOffset, int YOffset) { }
void CameraInterfaceUniversal::CameraSetHeight(int Height) { }
void CameraInterfaceUniversal::CameraSetWidth (int Width)  { }

void CameraInterfaceUniversal::CameraSetZoom    (int Zoom  ) { };
void CameraInterfaceUniversal::CameraSetGain    (float Gain) { } ;

void CameraInterfaceUniversal::CameraSetExposure(float Exposure) { }
void CameraInterfaceUniversal::CameraSetRegion  (int XOffset, int YOfffset, int width, int height ) {};



