#ifndef CAMERA_IMAGE_STORAGE_H
#define CAMERA_IMAGE_STORAGE_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "interface_image_source.h"
#include <QDebug>
#include <QTimer>
#include <mutex>
#include "debug_output_filter.h"

template<typename TYPE_CAMERA>
  class CameraImageStorage: public SourceImageInterface
  {
    public:
    explicit CameraImageStorage(TYPE_CAMERA* CameraDevice, std::pair<int,int> Size) 
                { SizeImage = Size; Camera = CameraDevice; initStorage(); };

            ~CameraImageStorage() {deinitStorage(); };

    TYPE_CAMERA* Camera = nullptr;

    void initStorage();
    void deinitStorage(){};
    bool isInit()       { return !Buffers.empty(); };

    static std::vector<cv::Mat> Buffers;
    static std::vector<cv::Mat>::iterator BufferToWrite;
    static std::mutex lockBuffer;
    void reset() { lockBuffer.unlock(); } 

           std::vector<cv::Mat>::iterator BufferToRead;
           cv::Mat ImageToProcess;

    static  QImage ImageToDisplay;
    static std::pair<int,int> SizeImage; 

    static void putNewFrameToStorage(cv::Mat& Frame);

     int getAvailableFrames() override { return BufferToWrite - BufferToRead; }
    bool isFrameAvailable()   override { return BufferToWrite != BufferToRead;};
    bool empty()              override { return BufferToWrite == BufferToRead;};
    bool switchToNextFrame()  override;
    void skipFrames()         override;

    std::pair<int,int> getSizeImage() override { return SizeImage; };

              cv::Mat& getImageToProcess() override { switchToNextFrame(); return ImageToProcess; }
                void   getImageToProcess(cv::Mat& ImageDst) override  { switchToNextFrame(); ImageDst = ImageToProcess.clone(); };
  };

template<typename TYPE_CAMERA>   std::pair<int,int> CameraImageStorage<TYPE_CAMERA>::SizeImage; 
template<typename TYPE_CAMERA>             QImage   CameraImageStorage<TYPE_CAMERA>::ImageToDisplay;
template<typename TYPE_CAMERA> std::vector<cv::Mat> CameraImageStorage<TYPE_CAMERA>::Buffers;
template<typename TYPE_CAMERA> std::vector<cv::Mat>::iterator CameraImageStorage<TYPE_CAMERA>::BufferToWrite;
template<typename TYPE_CAMERA> std::mutex CameraImageStorage<TYPE_CAMERA>::lockBuffer;

template<typename TYPE_CAMERA>
void CameraImageStorage<TYPE_CAMERA>::initStorage()
{
  if(Buffers.empty()) 
  {
    for (int i = 0; i < 100; i++) Buffers.push_back(cv::Mat(SizeImage.second,SizeImage.first,CV_8UC1)); 

    ImageToDisplay = QImage(SizeImage.first,SizeImage.second,QImage::Format_Grayscale8);
  }
                  BufferToWrite = Buffers.begin(); 
                   BufferToRead = Buffers.begin();
}


template<typename TYPE_CAMERA>
void CameraImageStorage<TYPE_CAMERA>::skipFrames()
{
  BufferToRead = BufferToWrite-1; if(BufferToWrite == Buffers.begin()) BufferToRead = Buffers.end()-1;
}


template<typename TYPE_CAMERA>
bool CameraImageStorage<TYPE_CAMERA>::switchToNextFrame() 
{
   //std::lock_guard<std::mutex> locker(lockBuffer);

   if(BufferToRead == BufferToWrite) return false; 
   if(getAvailableFrames() > 3) skipFrames();
   
   ImageToProcess = *BufferToRead; 
                     BufferToRead++; 
                  if(BufferToRead == Buffers.end()) BufferToRead = Buffers.begin(); 
   return true;
}


template<typename TYPE_CAMERA>
void CameraImageStorage<TYPE_CAMERA>::putNewFrameToStorage(cv::Mat& Frame)
{

                           //std::lock_guard<std::mutex> locker(lockBuffer);

                          *BufferToWrite = Frame.clone(); 
   ImageToDisplay = QImage(BufferToWrite->data,Frame.cols,Frame.rows,QImage::Format_Grayscale8);

   //qDebug() << OutputFilter::Filter(100) << "PUT FRAME: " << ImageToDisplay.width() << ImageToDisplay.height();

                           BufferToWrite++; 
                        if(BufferToWrite == Buffers.end()) BufferToWrite = Buffers.begin();

  SizeImage.first  = Frame.cols; 
  SizeImage.second = Frame.rows;

}

//qDebug() << "[ SKIP FRAMES ]";
//qDebug() << "[ GET FRAME ]";
//qDebug() << "[ PUT NEW FRAME ]";

#endif 
