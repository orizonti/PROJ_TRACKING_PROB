#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"
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
#include <debug_output_filter.h>
#include <QThread>
#include "image_processing_node.h"


int ModuleImageProcessing::CountModules = 0;
std::vector<ModuleImageProcessing*> ModuleImageProcessing::Modules;

std::mutex ModuleImageProcessing::MutexImageAccess;

ModuleImageProcessing::ModuleImageProcessing(QObject* parent) : QObject(parent) 
{
    CountModules++;
    NumberModule = CountModules;
    //WatcherModules.AppendModule(this);
RectsObject[0] = cv::Rect(0,0,ROI_SIZE,ROI_SIZE);
RectsObject[1] = cv::Rect(0,0,ROI_SIZE,ROI_SIZE);

CoordsObject[0] = QPair<float,float>(80,80); 
CoordsObject[1] = QPair<float,float>(80,80); 
ImageToDisplay = QImage(400,400,QImage::Format_Grayscale8); ImageToDisplay.fill(Qt::black);

         timerProcessImage.setInterval(15);
connect(&timerProcessImage, SIGNAL(timeout()), this, SLOT(SlotProcessImage()));
};


const std::vector<QPair<int,int>>& ModuleImageProcessing::getPoints()  
{
   CoordsImage[0] = CoordsObject[0];
   CoordsImage[1] = CoordsObject[1];
   return CoordsImage;
}

const std::vector<QRect>& ModuleImageProcessing::getRects()  
{
   RectsImage[0].setRect(RectsObject[0].x, RectsObject[0].y, RectsObject[0].width, RectsObject[0].height);
   RectsImage[1].setRect(CoordsObject[0].first-40, CoordsObject[0].second-40, 80, 80);
   return RectsImage;
}

const QString& ModuleImageProcessing::getInfo()  { return ProcessInfo; }

cv::Mat& ModuleImageProcessing::getImageToProcess() { return ImageProcessing; }

QImage& ModuleImageProcessing::getImageToDisplay()
{
    MutexImageAccess.lock();
    ImageToDisplay = QImage(ImageProcessing.data,   //GRAY IMAGE
                            ImageProcessing.cols,
                            ImageProcessing.rows,
           static_cast<int>(ImageProcessing.step),
              QImage::Format_Grayscale8 );

    MutexImageAccess.unlock();
    return ImageToDisplay;
}

void ModuleImageProcessing::getImageToProcess(cv::Mat& ImageDst)
{
    MutexImageAccess.lock();
    ImageDst = ImageProcessing.clone();
    MutexImageAccess.unlock();
}

void ModuleImageProcessing::getImageToDisplay(QImage& ImageDst)
{
  if(ImageProcessing.empty()) return; 

  if(ImageDst.width() != ImageProcessing.cols || ImageDst.height() != ImageProcessing.rows) 
     ImageDst = QImage(ImageProcessing.cols,ImageProcessing.rows,QImage::Format_ARGB32);

    //if(ImageProcessing.empty()) { ImageDst = ImageToDisplay.copy(); return; }

    MutexImageAccess.lock();
    uint8_t* RowData = 0;
    QRgb*    ImageDest = 0;
		for (int row = 0; row < this->ImageProcessing.rows; row++)
		{
			RowData = this->ImageProcessing.ptr<uint8_t>(row);
      ImageDest = (QRgb *)ImageDst.scanLine(row);

			for (int col = 0; col < this->ImageProcessing.cols; col++)
			{
          ImageDest++; *ImageDest = qRgb(RowData[col], RowData[col], RowData[col]);
			}
		}

    MutexImageAccess.unlock();
}



//==================================================================================
void ModuleImageProcessing::linkToModule(std::shared_ptr<SourceImageInterface> Source)
{
   if(isLinked()) return; SourceImage = Source; 
}

void ModuleImageProcessing::linkToModule(std::shared_ptr<ModuleImageProcessing> Dst)
{
   this->setLink(Dst.get()); Dst->FLAG_SLAVE_MODE = true; Dst->FLAG_TRACK_MODE = true;
}


void operator|(std::shared_ptr<ModuleImageProcessing > Source, std::shared_ptr<ModuleImageProcessing> Dst)
{
  qDebug() << Source->TAG_NAME << " [ LINK TO ] " << Dst->TAG_NAME;
  Source->linkToModule(Dst);
}

std::shared_ptr<ModuleImageProcessing> operator|(std::shared_ptr<SourceImageInterface> Source, std::shared_ptr<ModuleImageProcessing> Rec)
{
    //Rec->linkToModule(Source->getImageSourceChannel()); return Rec;
    Rec->linkToModule(Source->getImageSourceChannel()); return Rec;
}


bool ModuleImageProcessing::IsROIValid(cv::Rect& ROI)
{
const int& ImageWidth = ImageInput.cols;
const int& ImageHeight = ImageInput.rows;

if((ROI.x + ROI.width + 2) > ImageWidth) return false;
if((ROI.y + ROI.height + 2) > ImageHeight) return false;
if (ROI.x < 0) return false;
if (ROI.y < 0) return false;

return true;
}

void ModuleImageProcessing::CheckCorrectROI(cv::Rect& ROI)
{
const int ImageWidth = ImageInput.cols;
const int ImageHeight = ImageInput.rows;
if(ROI.width > ImageWidth) ROI.width = ImageWidth/2;
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


void ModuleImageProcessing::SetProcessingRegim(int Regim)       { }

void ModuleImageProcessing::SetModuleEnabled(bool StartStop) 
{ 
      this->PassBlocked = !StartStop;
   emit SignalBlockEnabled(StartStop);
}

void ModuleImageProcessing::SetThreshold(int Thresh)  { this->Threshold = Thresh; }

std::pair<float,float> ModuleImageProcessing::getFramePeriod() { return std::pair<float,float>(FrameMeasureInput.FramePeriod, 
                                                                                               FrameMeasureProcess.FramePeriod);};

void ModuleImageProcessing::SetImageParam(int MinWeight, int MaxWeight, int Distortion)
{
    MinImageWeight = MinWeight;
    MaxImageWeight = MaxWeight;
    MaxImageDistortion = Distortion; 
}


void ModuleImageProcessing::SlotResetProcessing()
{
  FLAG_TRACK_MODE = false;
  FLAG_OBJECT_HOLD = false;
  CoordsObject[0] = std::pair<double,double>(0,0);
  SetLowFrequencyProcessing();
}

void ModuleImageProcessing::SlotStopProcessing()
{
   qDebug() << TAG_NAME << "[ STOP PROCESSING ]" << QThread::currentThread();
   timerProcessImage.stop();
}


void ModuleImageProcessing::setInput(const QPair<float,float>& Coord)
{
  qDebug() << OutputFilter::Filter(20) << TAG_NAME << "[ SLAVE INPUT ]" << Coord.first << Coord.second;
  CoordsObject[1] = Coord; 
  RectsObject[0] = cv::Rect(CoordsObject[1].first  - ROI_SIZE/2, 
                            CoordsObject[1].second - ROI_SIZE/2 , ROI_SIZE, ROI_SIZE);
};

//void ModuleImageProcessing::SetSlaveMode(ModuleImageProcessing* Master)
//{
//   Master->setLink(this); FLAG_SLAVE_MODE = true; FLAG_TRACK_MODE = true;
//}

//auto max_element_it = std::max_element(ContourAreas.begin(), ContourAreas.end());
//NumberMaxContour = std::distance(ContourAreas.begin(), max_element_it);
//qDebug() << "HIERARCHY";
//qDebug() << "=======================================";
//qDebug() << "NUMBER: " << n++ << "NEXT: " << ContInfo[0] 
//                              << "PREV: " << ContInfo[1]  
//                              << "FIRST:" << ContInfo[2]
//                              << "PARenT:" << ContInfo[3];
//qDebug() << "=======================================";
//=====================================

cv::Rect& operator*(cv::Rect& rect, float Scale)
{
  rect.x -= (rect.width*Scale - rect.width)/2;
  rect.y -= (rect.height*Scale - rect.height)/2;
  rect.width *= Scale;
  rect.height *= Scale;
  return rect;
}

cv::Rect& operator*(float Scale, cv::Rect& rect)
{
   return rect*Scale;
}

