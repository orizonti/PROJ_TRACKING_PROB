#include "interface_camera_rtsp.h"
#include "interface_image_source.h"
#include <QDebug>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include "debug_output_filter.h"


using CameraStorageType = CameraImageStorage<CameraInterfaceUniversal>;
std::shared_ptr<SourceImageInterface> CameraInterfaceUniversal::getImageSourceChannel() 
{ 
  auto ChannelStore = ImageStore.back(); 
  ImageStore.push_back(std::make_shared<CameraStorageType>(this, SizeImage));
  return ChannelStore;
}

void CameraInterfaceUniversal::slotStartStream() { timerGetFrame.start(10); }
void CameraInterfaceUniversal::slotStopStream()  { timerGetFrame.stop(); }

CameraInterfaceUniversal::CameraInterfaceUniversal(std::string strVideoSource, QString NAME) : TAG_NAME(NAME)
{
  capture.set(cv::CAP_GSTREAMER,3);
  //capture.set(cv::CAP_FFMPEG,3);
  capture.open(strVideoSource);
  
  qDebug()  << TAG_NAME << "[ OPEN VIDEO SOURCE ]" << strVideoSource.c_str();
  if (!capture.isOpened()) qDebug()  << "[ ERROR ] CANNOT OPEN VIDEO SOURCE: " << strVideoSource.c_str();
  QObject::connect(&timerGetFrame, SIGNAL(timeout()),this, SLOT(slotGetFrame()));

  //cv::namedWindow("test");

  ImageStore.push_back(std::make_shared<CameraStorageType>(this, SizeImage));
}

CameraInterfaceUniversal::~CameraInterfaceUniversal() { qDebug() << "[ THERMAL CAMERA INTERFACE DEINIT]" << TAG_NAME; }

QImage& CameraInterfaceUniversal::getImageToDisplay() { return CameraStorageType::ImageToDisplay; }

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

        //cv::resize(inputImage,inputImageSmall,cv::Size(SizeImage.first,SizeImage.second));
        cv::cvtColor(inputImage,inputImageGray,cv::COLOR_RGB2GRAY);
        //cv::imshow("test", inputImageGray);
        //inputImage.convertTo(inputImageGray, CV_8UC1, 1, 0); 
        //inputImageGray = inputImage;

        //qDebug() << OutputFilter::Filter(50) << "GET IMAGE DEPTH: " << inputImageGray.elemSize() << inputImageGray.elemSize1() << inputImageGray.cols << inputImageGray.rows;

        mutexStorage.lock(); 
        CameraStorageType::putNewFrameToStorage(inputImageGray); 
        mutexStorage.unlock();
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
void CameraInterfaceUniversal::initCamera() { }
void CameraInterfaceUniversal::deinitCamera() { }

void CameraInterfaceUniversal::CameraSetHeight(int Height) { }
void CameraInterfaceUniversal::CameraSetWidth(int Width) { }
void CameraInterfaceUniversal::CameraSetOffset(int XOffset, int YOffset) { }
void CameraInterfaceUniversal::CameraSetSize(int Width, int Height) { }
void CameraInterfaceUniversal::CameraSetExposure(float Exposure) { }

