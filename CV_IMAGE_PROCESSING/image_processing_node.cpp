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

ModuleImageProcessing::ModuleImageProcessing(QObject* parent) : ImageSourceInterface(parent) 
{
    CountModules++;
    NumberModule = CountModules;
    //WatcherModules.AppendModule(this);
RectsObject[0] = cv::Rect(0,0,ROI_SIZE,ROI_SIZE);
RectsObject[1] = cv::Rect(0,0,ROI_SIZE,ROI_SIZE);

CoordsObject[0] = QPair<double,double>(80,80); 
CoordsObject[1] = QPair<double,double>(80,80); 
ImageToDisplay = QImage(400,400,QImage::Format_Grayscale8); ImageToDisplay.fill(Qt::black);

         timerProcessImage.setInterval(15);
connect(&timerProcessImage, SIGNAL(timeout()), this, SLOT(SlotProcessImage()));
};


std::vector<QPair<int,int>>& ModuleImageProcessing::GetPoints()  
{
   CoordsImage[0] = CoordsObject[0];
   CoordsImage[1] = CoordsObject[1];
   return CoordsImage;
}

std::vector<QRect>& ModuleImageProcessing::GetRects()  
{
   RectsImage[0].setRect(RectsObject[0].x, RectsObject[0].y, RectsObject[0].width, RectsObject[0].height);
   RectsImage[1].setRect(CoordsObject[0].first-20, CoordsObject[0].second-20, 40, 40);
   return RectsImage;
}

QString& ModuleImageProcessing::GetInfo()  { return ProcessInfo; }

cv::Mat& ModuleImageProcessing::GetImageToProcess() { return ImageProcessing; }

QImage& ModuleImageProcessing::GetImageToDisplay()
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

void ModuleImageProcessing::GetImageToProcess(cv::Mat& ImageDst)
{
    MutexImageAccess.lock();
    ImageDst = ImageProcessing.clone();
    MutexImageAccess.unlock();
}

void ModuleImageProcessing::GetImageToDisplay(QImage& ImageDst)
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
void ModuleImageProcessing::LinkToModule(std::shared_ptr<ImageSourceInterface> Source)
{
   if(isLinked()) return; SourceImage = Source; 
}

std::shared_ptr<ModuleImageProcessing> operator|(std::shared_ptr<ImageSourceInterface> Source, std::shared_ptr<ModuleImageProcessing> Rec)
{
    //Rec->LinkToModule(Source->GetImageSourceChannel()); return Rec;
    Rec->LinkToModule(Source); return Rec;
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
void ModuleImageProcessing::StartStopProcessing(bool StartStop) { }
void ModuleImageProcessing::SetThreshold(int Thresh)  {qDebug() << "THRES: " << Thresh; this->Threshold = Thresh; }


std::pair<float,float> ModuleImageProcessing::GetFramePeriod() { return std::pair<float,float>(FrameMeasureInput.FramePeriod, 
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


void ModuleImageProcessing::SetInput(const QPair<double,double>& Coord)
{
  CoordsObject[1] = Coord; 
  RectsObject[0] = cv::Rect(CoordsObject[1].first  - ROI_SIZE/2, 
                            CoordsObject[1].second - ROI_SIZE/2 , ROI_SIZE, ROI_SIZE);
};

void ModuleImageProcessing::SetSlaveMode(ModuleImageProcessing* Master)
{
   Master->SetLink(this); FLAG_SLAVE_MOVE = true; FLAG_TRACK_MODE = true;
}

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

