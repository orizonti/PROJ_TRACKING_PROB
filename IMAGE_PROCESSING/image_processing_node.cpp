#include "engine_statistics.h"
#include "glib.h"
#include "gmodule.h"
#include "interface_pass_coord.h"
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <qnamespace.h>
#include <chrono>
#include <QThread>
#include "image_processing_node.h"
#include <debug_output_filter.h>
#include "register_settings.h"

ModuleImageProcessing::ModuleImageProcessing(QString name, QObject* parent) : QObject(parent), TAG_NAME(name)
{
         SizeROI = SettingsRegister::GetValue("PROCESSING_ROI1");
auto  resolution = SettingsRegister::GetPair ("CAMERA1_RESOLUTION"); 

RectsObject[0] = cv::Rect(resolution.first/2 - SizeROI/2, resolution.second/2 - SizeROI/2, SizeROI,SizeROI);
RectsObject[1] = cv::Rect(resolution.first/2 - SizeROI/2, resolution.second/2 - SizeROI/2, SizeROI,SizeROI);

CoordsObject[0] = QPair<float,float>(resolution.first/2,resolution.first/2); 
CoordsObject[1] = QPair<float,float>(resolution.first/2,resolution.first/2); 

ImageToDisplay = QImage(resolution.first,resolution.second,QImage::Format_ARGB32); ImageToDisplay.fill(Qt::black);

qDebug() << TAG_NAME << "[LOAD DEFAULT PARAMETERS ]" << "[ IMAGE ] " << resolution.first << resolution.first << "[ ROI ]" << SizeROI;

         timerProcessImage.setInterval(15);
connect(&timerProcessImage   , SIGNAL(timeout()), this, SLOT(SlotProcessImage()));

};

ModuleImageProcessing::ModuleImageProcessing(int width, int height, int size, QString name, QObject* parent) : QObject(parent), 
                                                                                                               TAG_NAME(name)
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

const QString& ModuleImageProcessing::getInfo()  { return INFO; }

cv::Mat& ModuleImageProcessing::getImageToProcess() { return ImageProcessing; }

void ModuleImageProcessing::getImageToProcess(cv::Mat& ImageDst)
{
    MutexImageAccess.lock();
    ImageDst = ImageProcessing.clone();
    MutexImageAccess.unlock();
}

void ModuleImageProcessing::getImageToDisplay(QImage& ImageDst)
{
  if(ImageOutput.empty()) return; 

  if(ImageDst.width() != ImageOutput.cols || ImageDst.height() != ImageOutput.rows) 
     ImageDst = QImage(ImageOutput.cols,ImageOutput.rows,QImage::Format_ARGB32);

    //if(ImageOutput.empty()) { ImageDst = ImageToDisplay.copy(); return; }

    MutexImageAccess.lock();
    uint8_t* RowData = 0;
    QRgb*    ImageDest = 0;
		for (int row = 0; row < this->ImageOutput.rows; row++)
		{
			RowData = this->ImageOutput.ptr<uint8_t>(row);
      ImageDest = (QRgb *)ImageDst.scanLine(row);

			for (int col = 0; col < this->ImageOutput.cols; col++)
			{
          ImageDest++; *ImageDest = qRgb(RowData[col], RowData[col], RowData[col]);
			}
		}

    MutexImageAccess.unlock();
}

//==================================================================================

void ModuleImageProcessing::linkToModule(std::shared_ptr<ModuleImageProcessing> Dst)
{
   qDebug() << TAG_NAME << "[ LINK TO ]" << Dst->TAG_NAME;
   QObject::connect(this, SIGNAL(signalCoord(std::pair<float,float>)), Dst.get(), 
                          SLOT  (slotCoordInput  (std::pair<float,float>)), Qt::QueuedConnection);

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

if((ROI.x + ROI.width  + 2) > ImageWidth)  return false;
if((ROI.y + ROI.height + 2) > ImageHeight) return false;
if (ROI.x < 0) return false;
if (ROI.y < 0) return false;
               return true ;
}

void ModuleImageProcessing::CheckCorrectROI(cv::Rect& ROI)
{
const int ImageWidth  = (*ImageInput).cols;
const int ImageHeight = (*ImageInput).rows;
if(ROI.width  > ImageWidth) ROI.width = ImageWidth/2;
if(ROI.height > ImageHeight) ROI.height = ImageHeight/2;

auto OFFSET_X = (ROI.x + ROI.width ) - ImageWidth; if(OFFSET_X < 0) OFFSET_X = 0;
auto OFFSET_Y = (ROI.y + ROI.height) - ImageHeight; if(OFFSET_Y < 0) OFFSET_Y = 0;

   ROI.x -= OFFSET_X;
   ROI.y -= OFFSET_Y;

if(ROI.x < 0 ) ROI.x = 0;
if(ROI.y < 0 ) ROI.y = 0;
//qDebug() << "OUTPUT ROI: " << ROI.x << ROI.y << ROI.width << ROI.height << "IMAGE: " << ImageInput.cols << ImageInput.rows;

}

//==================================================================================

std::pair<float,float> ModuleImageProcessing::getTickPeriod() { return std::pair<float,float>(FrameMeasureInput.TickPeriod, 
                                                                                              FrameMeasureProcess.TickPeriod);};


void ModuleImageProcessing::SlotResetProcessing() { StateProcessing = StatesModule::Idle; }

void ModuleImageProcessing::SlotStopProcessing() 
{ timerProcessImage.stop(); qDebug() << TAG_NAME << "[ STOP PROCESSING ]" << QThread::currentThread(); }

void ModuleImageProcessing::SlotStartProcessing() 
{ timerProcessImage.start(); qDebug() << TAG_NAME << "[ START PROCESSING ]" << QThread::currentThread(); }



void ModuleImageProcessing::setInput(const QPair<float,float>& Coord)
{
  if(StateProcessing == StatesModule::WorkTrack && ModeProcessing == ModesModule::Master) return;

  CoordsObject[1] = Coord; 
  RectsObject[0] = cv::Rect(CoordsObject[1].first  - SizeROI/2, 
                            CoordsObject[1].second - SizeROI/2 , SizeROI, SizeROI);

  SetStateActive();
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

  //qDebug() << OutputFilter::Filter(20) << TAG_NAME << "[ SLAVE INPUT ]" << Coord.first << Coord.second;


