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

void CameraInterfaceUniversal::slotStartStream() { timerGetFrame.start(2); qDebug() << "[ START RTSP ]"; }
void CameraInterfaceUniversal::slotStopStream()  { timerGetFrame.stop(); }

CameraInterfaceUniversal::CameraInterfaceUniversal(std::string strVideoSource, QString NAME) : TAG_NAME(NAME)
{
 
  capture.open(strVideoSource, cv::CAP_GSTREAMER);
  //capture.set(cv::CAP_PROP_HW_DEVICE, 1);
  //capture.set(cv::CAP_PROP_BUFFERSIZE, 1);


  qDebug()  << TAG_NAME << "[ OPEN VIDEO SOURCE TEST ]" << strVideoSource.c_str();
  if (!capture.isOpened()) qDebug()  << "[ ERROR ] CANNOT OPEN VIDEO SOURCE: " << strVideoSource.c_str();
  QObject::connect(&timerGetFrame, SIGNAL(timeout()),this, SLOT(slotGetFrame()));

  //cv::namedWindow("test");

  ImageStore.push_back(std::make_shared<CameraStorageType>(this, SizeImage));
}

CameraInterfaceUniversal::~CameraInterfaceUniversal() { qDebug() << "[ THERMAL CAMERA INTERFACE DEINIT]" << TAG_NAME; }


void CameraInterfaceUniversal::getImageToDisplay(QImage& ImageDst) 
{ 
  //qDebug() << "RTSP DISPLAY IMAGE SIZE" << ImageStore->ImageToDisplay.size() << "NULL:" << ImageStore->ImageToDisplay.isNull();
//                               if(ImageStore->ImageToDisplay.isNull()) return;
    //mutexStorage.lock(); ImageDst = ImageStore->ImageToDisplay.copy(); mutexStorage.unlock();
    mutexStorage.lock(); ImageDst = CameraStorageType::ImageToDisplay; mutexStorage.unlock();
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
        cv::resize  (inputImage     ,inputImageSmall,cv::Size(SizeImage.first,SizeImage.second));
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



