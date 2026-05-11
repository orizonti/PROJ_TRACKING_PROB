#include "interface_pass_coord.h"
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <qnamespace.h>
#include <QThread>
#include "image_processing_node.h"
#include <debug_output_filter.h>
#include "register_settings.h"


ModuleImageProcessing::ModuleImageProcessing(QString name, QObject* parent) : QObject(parent), TAG_NAME(name.toStdString())
{

RectsObject[0] = cv::Rect(SizeImage.first/2 - SizeROI/2, SizeImage.second/2 - SizeROI/2, SizeROI,SizeROI);
RectsObject[1] = cv::Rect(SizeImage.first/2 - SizeROI/2, SizeImage.second/2 - SizeROI/2, SizeROI,SizeROI);

CoordsObject[0] = QPair<float,float>(SizeImage.first/2,SizeImage.first/2); 
CoordsObject[1] = QPair<float,float>(SizeImage.first/2,SizeImage.first/2); 

ImageToDisplay = QImage(SizeImage.first,SizeImage.second,QImage::Format_RGB888); ImageToDisplay.fill(Qt::black);


         timerProcessImage.setInterval(15);
connect(&timerProcessImage   , SIGNAL(timeout()), this, SLOT(SlotProcessImage()));

QObject::connect(this,SIGNAL(signalStart()), this, SLOT(SlotStartProcessing()),Qt::QueuedConnection);
QObject::connect(this,SIGNAL(signalStop()) , this, SLOT(SlotStopProcessing()),Qt::QueuedConnection);
QObject::connect(this,SIGNAL(signalReset()), this, SLOT(SlotResetProcessing()),Qt::QueuedConnection);

};

void ModuleImageProcessing::printInfo()
{
qDebug() << TAG_NAME.c_str() << "[ WAIT IMAGE ]" << SizeImage.first << SizeImage.second << "[ ROI ]" << SizeROI;
}

ModuleImageProcessing::ModuleImageProcessing(int width, int height, int size, QString name, QObject* parent) : QObject(parent), 
                                                                                                               TAG_NAME(name.toStdString())
{
         SizeROI = size;

RectsObject[0] = cv::Rect(width/2 - SizeROI/2, height/2 - SizeROI/2, SizeROI,SizeROI);
RectsObject[1] = cv::Rect(width/2 - SizeROI/2, height/2 - SizeROI/2, SizeROI,SizeROI);

CoordsObject[0] = QPair<float,float>(width/2, height/2); 
CoordsObject[1] = QPair<float,float>(width/2, height/2); 

ImageToDisplay = QImage(width,height,QImage::Format_ARGB32); ImageToDisplay.fill(Qt::black);

         timerProcessImage.setInterval(15);
connect(&timerProcessImage   , SIGNAL(timeout()), this, SLOT(SlotProcessImage()));
qDebug() << TAG_NAME << "[ IMAGE ] " << width << height << "[ ROI ]" << size;

QObject::connect(this,SIGNAL(signalStart()), this, SLOT(SlotStartProcessing()),Qt::QueuedConnection);
QObject::connect(this,SIGNAL(signalStop()), this, SLOT(SlotStopProcessing()),Qt::QueuedConnection);
};


const std::vector<QPair<int,int>>& ModuleImageProcessing::getPoints()  
{
   CoordsImage[0] = CoordsObject[0];
   CoordsImage[1] = CoordsObject[1];
   return CoordsImage;
}

const std::vector<QRect>& ModuleImageProcessing::getRects()  
{
   RectsImage[0].setRect(RectsObject[0].x, 
                         RectsObject[0].y, RectsObject[0].width, 
                                           RectsObject[0].height);
   RectsImage[1].setRect(CoordsObject[0].first-40 , 
                         CoordsObject[0].second-40, 80, 80);
   return RectsImage;
}

const std::string& ModuleImageProcessing::getInfo()  { return INFO; }

cv::Mat& ModuleImageProcessing::getImageToProcess() { return ImageProcessing; }

void ModuleImageProcessing::getImageToProcess(cv::Mat& ImageDst)
{
    MutexImageAccess.lock();
    ImageDst = ImageProcessing.clone();
    MutexImageAccess.unlock();
}

void ModuleImageProcessing::getImageToDisplay(QImage& ImageDst)
{
  std::lock_guard<std::mutex> locker(MutexImageAccess);
  if(ImageOutput.empty())  return;  

  if(ImageDst.width() != ImageOutput.cols || ImageDst.height() != ImageOutput.rows) 
       ImageDst = QImage(ImageOutput.cols,ImageOutput.rows,QImage::Format_RGB888);


        for (int y = 0; y < this->ImageOutput.rows; ++y) 
        {
            for (int x = 0; x < this->ImageOutput.cols; ++x) 
            {
                ImageDst.setPixel(x, y, qRgb(this->ImageOutput.at<uchar>(y, x), 
                                             this->ImageOutput.at<uchar>(y, x), 
                                             this->ImageOutput.at<uchar>(y, x)));
            }
        }

}

//QImage matToQImage(const cv::Mat& mat) {
//    if (mat.type() == CV_8UC1) {
//        QImage image(mat.cols, mat.rows, QImage::Format_Grayscale8);
//        for (int y = 0; y < mat.rows; ++y) {
//            for (int x = 0; x < mat.cols; ++x) {
//                image.setPixel(x, y, qRgb(mat.at<uchar>(y, x), mat.at<uchar>(y, x), mat.at<uchar>(y, x)));
//            }
//        }
//        return image;
//    }
//    return QImage();
//}

//==================================================================================

void ModuleImageProcessing::linkToModule(std::shared_ptr<ModuleImageProcessing> Dst)
{
   qDebug() << TAG_NAME.c_str() << "[ LINK TO ]" << Dst->TAG_NAME.c_str();
   *this | *Dst; Links.push_back(Dst);
}

void operator|(std::shared_ptr<ModuleImageProcessing > Source, std::shared_ptr<ModuleImageProcessing> Dst)
{
  Source->linkToModule(Dst); 
}

//=======================================================================================
void ModuleImageProcessing::linkToModule(std::shared_ptr<SourceImageInterface> Source)
{
   if(isLinkedSource()) return; SourceImage = Source; 
}

std::shared_ptr<ModuleImageProcessing> operator|(std::shared_ptr<SourceImageInterface> Source, std::shared_ptr<ModuleImageProcessing> Rec)
{
    Rec->linkToModule(Source->getImageSourceChannel()); return Rec;
}
//=======================================================================================


bool ModuleImageProcessing::IsROIValid(cv::Rect& ROI)
{
const int& ImageWidth  = (*ImageInput).cols;
const int& ImageHeight = (*ImageInput).rows;

  if((ROI.x + ROI.width  + 2) > ImageWidth  || 
     (ROI.y + ROI.height + 2) > ImageHeight ||
                                 ROI.x <= 0 || 
                                 ROI.y <= 0) return false;
  return true ;
}

void ModuleImageProcessing::CheckCorrectROI(cv::Rect& ROI)
{
const int ImageWidth  = (*ImageInput).cols;
const int ImageHeight = (*ImageInput).rows;
if(ROI.width  > ImageWidth /2) ROI.width  = ImageWidth /2;
if(ROI.height > ImageHeight/2) ROI.height = ImageHeight/2;

auto OFFSET_X = (ROI.x + ROI.width ) - ImageWidth;  if(OFFSET_X < 0) OFFSET_X = 0;
auto OFFSET_Y = (ROI.y + ROI.height) - ImageHeight; if(OFFSET_Y < 0) OFFSET_Y = 0;

   ROI.x -= OFFSET_X;
   ROI.y -= OFFSET_Y;

if(ROI.x <= 0 ) ROI.x = 1;
if(ROI.y <= 0 ) ROI.y = 1;
//qDebug() << "OUTPUT ROI: " << ROI.x << ROI.y << ROI.width << ROI.height << "IMAGE: " << ImageInput.cols << ImageInput.rows;

}

//==================================================================================

std::pair<float,float> ModuleImageProcessing::getTickPeriod() { return std::pair<float,float>(FrameMeasureInput.TickPeriod, 
                                                                                              FrameMeasureProcess.TickPeriod);};


void ModuleImageProcessing::SlotResetProcessing() 
{ 
  qDebug() << TAG_NAME.c_str() << "[RESET PROCESSING]";
  StateProcessing = StatesModule::Idle; 
  MutexImageAccess.unlock();
  MutexInput.unlock();
  timerProcessImage.start(); 
}

void ModuleImageProcessing::SlotStopProcessing() 
{ 
  timerProcessImage.stop(); 
  StateProcessing = StatesModule::Idle;
  qDebug() << TAG_NAME.c_str() << "[ STOP PROCESSING ]" << QThread::currentThread(); 
}

void ModuleImageProcessing::SlotStartProcessing() 
{ 
  StateProcessing = StatesModule::WorkTrack;
  timerProcessImage.start(); 
  qDebug() << TAG_NAME.c_str() << "[ START PROCESSING ]" << QThread::currentThread(); 
}



void ModuleImageProcessing::setInput(const QPair<float,float>& Coord)
{
  if(StateProcessing == StatesModule::WorkTrack && ModeProcessing == ModesModule::Master) return;

  std::lock_guard<std::mutex> guard(MutexInput);

  CoordsObject[1] = Coord; 
  RectsObject[0] = cv::Rect(CoordsObject[1].first  - SizeROI/2, 
                            CoordsObject[1].second - SizeROI/2 , SizeROI, SizeROI);

  StateProcessing = StatesModule::WorkTrack;
};

ModuleImageProcessing& operator>>(const cv::Mat& Image, ModuleImageProcessing& Module)
{
  Module.SlotProcessImage(Image);
  return Module;
}

std::shared_ptr<ModuleImageProcessing> operator>>(const cv::Mat& Image, std::shared_ptr<ModuleImageProcessing> Module)
{
  Module->SlotProcessImage(Image);
  return Module;
}

void ModuleImageProcessing::setEnable(bool OnOff, uint16_t Number)
{
  switch(Number)
  {
    case 0: if(OnOff) emit signalStart(); else emit signalStop(); break; //ENABLE SIGNAL
    case 1: emit signalStop(); break;                                       //FAULT SIGNAL
  }
}

void ModuleImageProcessing::moveToThread(QThread* thread)
{

          QObject::moveToThread(thread);
 timerProcessImage.moveToThread(thread);
   NodeSignalFault.moveToThread(thread);
  NodeSignalEnable.moveToThread(thread);

  qDebug() << TAG_NAME.c_str() << "[ MOVE TO THREAD ]" << this->thread();
}

  //qDebug() << OutputFilter::Filter(20) << TAG_NAME << "[ SLAVE INPUT ]" << Coord.first << Coord.second;


