#include "interface_camera_rtsp.h"
#include "interface_image_source.h"
#include <QDebug>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <debug_output_filter.h>
#include <QThread>


using CameraStorageType = CameraImageStorage<CameraInterfaceUniversal>;
std::shared_ptr<SourceImageInterface> CameraInterfaceUniversal::getImageSourceChannel() 
{ 

  auto ChannelStore = ImageStore.back(); 
                      ImageStore.push_back(std::make_shared<CameraStorageType>(this, SIZE_CAMERA));
  qDebug() << "GET IMAGE SOURCE STORE" << ImageStore.size();
  return ChannelStore;
}

void CameraInterfaceUniversal::slotStartStream() { timerGetFrame.start(10); qDebug() << "[ START RTSP ]"; }
void CameraInterfaceUniversal::slotStopStream()  { timerGetFrame.stop(); }

CameraInterfaceUniversal::CameraInterfaceUniversal(std::string strVideoSource, uint32_t Number, QString NAME) : TAG_NAME(NAME.toStdString())
{
 
  qDebug() << TAG_NAME << "[CAMERA RESOLUTION]" << SIZE_CAMERA.first << SIZE_CAMERA.second;
  qDebug() << TAG_NAME << "[PROCESS SIZE]"      << SIZE_ROI.first    << SIZE_ROI.second;
  qDebug() << TAG_NAME << "[PROCESS OFFSET]"    << OFFSET_ROI.first  << OFFSET_ROI.second;

  capture.open(strVideoSource, cv::CAP_GSTREAMER);

                           qDebug() << TAG_NAME << "[LINK]" << strVideoSource.c_str();
  if (!capture.isOpened()) qDebug() << "[ERROR] [BAD LINK]" << strVideoSource.c_str();

  QObject::connect(&timerGetFrame, SIGNAL(timeout()),this, SLOT(slotGetFrame()));

  QObject::connect(this,SIGNAL(signalStart()), this, SLOT(slotStartStream()), Qt::QueuedConnection);
  QObject::connect(this,SIGNAL(signalStop()) , this, SLOT(slotStopStream()) , Qt::QueuedConnection);
  QObject::connect(this,SIGNAL(signalReset()), this, SLOT(slotReset())      , Qt::QueuedConnection);

  ImageStore.push_back(std::make_shared<CameraStorageType>(this, SIZE_CAMERA));
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

std::string getDepthString(const cv::Mat& mat) 
{
    int depth = mat.depth();
    switch (depth) {
        case CV_8U:  return "8-bit Unsigned (CV_8U)";
        case CV_8S:  return "8-bit Signed (CV_8S)";
        case CV_16U: return "16-bit Unsigned (CV_16U)";
        case CV_16S: return "16-bit Signed (CV_16S)";
        case CV_32S: return "32-bit Signed (CV_32S)";
        case CV_32F: return "32-bit Float (CV_32F)";
        case CV_64F: return "64-bit Float (CV_64F)";
        default:     return "Unknown depth";
    }
}

void CameraInterfaceUniversal::slotGetFrame()
{
  bool isFrameGrabbed = capture.grab();
  if (!isFrameGrabbed) return; 

       isFrameGrabbed = capture.retrieve(inputImage);
  if (!isFrameGrabbed) return; 


    FrameMeasureInput.PushTick();
  FrameMeasureProcess.PushTick();

      
//                  cv::rotate(inputImage, inputImageRotated,cv::ROTATE_90_COUNTERCLOCKWISE);
//                     inputImageResized = inputImageRotated(rectCrop); 

               inputImageProcess = inputImage;
  cv::cvtColor(inputImageProcess, inputImageGray,cv::COLOR_BGR2GRAY); inputImageProcess = inputImageGray;

  CameraStorageType::putNewFrameToStorage(inputImageProcess); 

  FrameMeasureProcess.PushTick();
}
//cv::resize  (inputImage ,inputImageResized,cv::Size(SIZE_CAMERA.first,SIZE_CAMERA.second));
//             inputImageResized = inputImage(rectCrop);
//qDebug() << OutputFilter::Filter(20) 
//         << "GET IMAGE: " << inputImageProcessed.cols << inputImageProcessed.rows  
//                          << inputImageProcessed.depth() << inputImageProcessed.channels();
//qDebug() << OutputFilter::Filter(10) << "[ RTSP FRAME PERIOD2 ]" << FrameMeasureInput.getMilliseconds();

void CameraInterfaceUniversal::slotEndWork() 
{ 
  qDebug() << TAG_NAME << "[ STOP CAMERA ]";
  slotStopStream(); deinitCamera(); 
  delete this;
}


std::pair<int,int> CameraInterfaceUniversal::getSizeImage() { return ImageStore[0]->getSizeImage(); };

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

void CameraInterfaceUniversal::moveToThread(QThread* thread)
{
       QObject::moveToThread(thread);
  timerGetFrame.moveToThread(thread);
  qDebug() << TAG_NAME.c_str() << "[ MOVE TO THREAD ]" << this->thread();
}


