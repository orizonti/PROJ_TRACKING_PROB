#include "cv_image_processing.h"
#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"
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
OutputFilter Filter10{100};
OutputFilter Filter100{100};
OutputFilter Filter50{20};
OutputFilter Filter200{200};
OutputFilter Filter200_2{200};
OutputFilter Filter5_2{5};
OutputFilter Filter5_22{5};

int ImageTrackerCentroid::NumberChannels = 0;

bool operator==(std::pair<int,int> pair, std::pair<int,int> pair2) { return pair.first == pair2.first && pair.second == pair2.second; };
bool operator==(std::pair<int,int> pair, int value) { return pair.first == value && pair.second == value; };
bool operator!=(std::pair<int,int> pair, std::pair<int,int> pair2) { return pair.first != pair2.first && pair.second != pair2.second; };
bool operator!=(std::pair<int,int> pair, int value) { return pair.first != value && pair.second != value; };

ImageTrackerCentroid::ImageTrackerCentroid(QObject* parent) : ModuleImageProcessing(parent)
{
  qRegisterMetaType<const cv::Mat&>();
  //std::cout << cv::getBuildInformation() << std::endl;
  QObject::connect(&timerDisplay,SIGNAL(timeout()),this, SLOT(SlotDisplayProcessingImage()));
  NumberChannel = ++NumberChannels;


    //cv::Mat kernel1 = (cv::Mat_<double>(3,3) << -1, -1, -1, -1, 9, -1, -1, -1, -1);
    cv::Mat kernel1 = (cv::Mat_<double>(3,3) << 0, 0, 0, 0, 1, 0, 0, 0, 0);
    FilterSharpen = [kernel1,this](cv::Mat& Image, cv::Mat& ImageOutput)
    { 
    filter2D(Image, ImageOutput, -1 , kernel1, cv::Point(-1, -1), 0, 4);
    };

    cv::Mat kernel2 = (cv::Mat_<double>(3,3) << -1, 0, 1, -2, 0, -2, -1, 0, 1);
    FilterSobel = [kernel2,this](cv::Mat& Image, cv::Mat& ImageOutput)
    { 
    filter2D(Image, ImageOutput, -1 , kernel2, cv::Point(-1, -1), 0, 4);
    };

    int erosion_size = 2;
    int dilation_size = 2;
    //int erosion_type = cv::MORPH_RECT;
    int erosion_type = cv::MORPH_CROSS;
    int dilation_type = cv::MORPH_RECT;

    cv::Mat element = getStructuringElement( erosion_type,
                        cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                        cv::Point( erosion_size, erosion_size ) );


    FilterErosion = [element,this](cv::Mat& Image, cv::Mat& ImageOutput)
    { 
    erode( Image, ImageOutput, element );
    };

}

ImageTrackerCentroid::~ImageTrackerCentroid()
{
 qDebug() << "DELETE IMAGE TRACKER CENTROID";
}

ImageTrackerTemplate::ImageTrackerTemplate(QObject* parent) : ImageTrackerCentroid(parent)
{
}

ImageTrackerTemplate::~ImageTrackerTemplate()
{
 qDebug() << "DELETE IMAGE TRACKER TEMPLATE";
}

const std::vector<QPair<int,int>>& ModuleImageProcessing::GetPoints()  
{
   CoordsImage[0] = CoordsObject[0];
   CoordsImage[1] = CoordsObject[1];
   return CoordsImage;
}

const std::vector<QRect>& ModuleImageProcessing::GetRects()  
{
   RectsImage[0].setRect(RectsObject[0].x, RectsObject[0].y, RectsObject[0].width, RectsObject[0].height);
   RectsImage[1].setRect(CoordsObject[0].first-20, CoordsObject[0].second-20, 40, 40);
   return RectsImage;
}

const QString& ModuleImageProcessing::GetInfo()  { return ProcessInfo; }

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
  if(ImageDst.width() != ImageProcessing.cols || ImageDst.height() != ImageProcessing.rows) 
     ImageDst = QImage(ImageProcessing.cols,ImageProcessing.rows,QImage::Format_ARGB32);

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

void ImageTrackerTemplate::CalcObjectTrajectory()
{
  TrajectoryIncrement.push_back(std::make_pair<float,float>(
                                               float(CoordsObject[0].first - Trajectory.back().first),
                                               float(CoordsObject[0].second - Trajectory.back().second)));
  auto Vel = TrajectoryIncrement.back();
 
  VelAimAvarage = std::accumulate(TrajectoryIncrement.end() - 10, 
                                  TrajectoryIncrement.end(),
                                 std::make_pair<float,float>(0,0),
                                 []( std::pair<float,float> acc, const std::pair<float,float>& val) mutable -> std::pair<float,float>
                                                                                      { acc.first += val.first/10.0;
                                                                                        acc.second += val.second/10.0;
                                                                                 return acc;}
                                 );


  Trajectory.erase(Trajectory.begin());
  Trajectory.push_back(std::make_pair(CoordsObject[0].first,CoordsObject[0].second));

  TrajectoryIncrement.erase(TrajectoryIncrement.begin());
}

void ImageTrackerTemplate::TrackImageTemplate(cv::Mat& Image)
{
  if(ImageTemplate.empty()) {qDebug() << TAG_NAME << " [ TRACKING TEMPLATE EMPTY ]"; return; }

                     ImageProcessing = Image(RectsObject[0]); 
  //=================================================================================
  int result_cols = Image.cols - ImageTemplate.cols + 1;
  int result_rows = Image.rows - ImageTemplate.rows + 1;

                       ProcessResult.create( result_rows, result_cols, CV_32FC1 );
  cv::matchTemplate( ImageProcessing, ImageTemplate, 
                                      ProcessResult, cv::TM_SQDIFF); 
        cv::normalize( ProcessResult, ProcessResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

                                double minVal; cv::Point minLoc;
                                double maxVal; cv::Point maxLoc;
        cv::minMaxLoc( ProcessResult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

  CoordsObject[0].first  = minLoc.x + RectsObject[0].x; 
  CoordsObject[0].second = minLoc.y + RectsObject[0].y; 
  //=================================================================================

  CalcObjectTrajectory();

  RectsObject[0] = cv::Rect(CoordsObject[0].first  + VelAimAvarage.first  - 10, 
                            CoordsObject[0].second + VelAimAvarage.second - 20, 80, 80);

  CheckCorrectROI(RectsObject[0]);

}

void ImageTrackerTemplate::FindImageTemplate(cv::Mat& Image)
{
  cv::medianBlur(Image,Image,5);
   cv::threshold(Image,Image,Threshold,256,cv::THRESH_BINARY);

  ContoursProcessor.SetImage(Image);
  RectsObject[0] = ContoursProcessor.GetMaxContourRect();
}

void ImageTrackerTemplate::SlotProcessImage(const cv::Mat& Image)
{
                             ImageProcessing = Image.clone(); 
    FilterBlotch.FilterImage(ImageProcessing, ImageProcessing); 

     FindImageTemplate(ImageProcessing); 
    TrackImageTemplate(ImageProcessing); 

    emit ImageSourceInterface::SignalNewImage(); if(ReceiverLinkCoord) PassTwoCoordClass::PassCoord();
}

void ImageTrackerTemplate::SlotProcessImage()
{
    MutexImageAccess.lock();
    ImageInput = SourceImage->GetImageToProcess().clone(); ImageProcessing = ImageInput;
    MutexImageAccess.unlock();

    FilterBlotch.FilterImage(ImageProcessing, ImageProcessing); 

      FindImageTemplate(ImageProcessing); 
    TrackImageTemplate(ImageProcessing); 

    emit ImageSourceInterface::SignalNewImage(); if(ReceiverLinkCoord) PassTwoCoordClass::PassCoord();
}


void ModuleImageProcessing::LinkToModule(std::shared_ptr<ImageSourceInterface> Source)
{
                    SourceImage = Source;
   QObject::connect(SourceImage.get(),SIGNAL(SignalNewImage()),this,SLOT(SlotProcessImage()), Qt::QueuedConnection);
}

std::shared_ptr<ModuleImageProcessing> operator|(std::shared_ptr<ImageSourceInterface> Source, std::shared_ptr<ModuleImageProcessing> Rec)
{
    Rec->LinkToModule(Source); return Rec;
}


bool ModuleImageProcessing::IsROIValid(cv::Rect& ROI)
{
const int& ImageWidth = ImageInput.cols;
const int& ImageHeight = ImageInput.rows;

if((ROI.x + ROI.width + 1) > ImageWidth) return false;
if((ROI.y + ROI.height + 1) > ImageHeight) return false;
if (ROI.x < 0) return false;
if (ROI.y < 0) return false;

return true;
}

void ModuleImageProcessing::CheckCorrectROI(cv::Rect& ROI)
{
const int& ImageWidth = ImageInput.cols;
const int& ImageHeight = ImageInput.rows;

const int OFFSET_X = ROI.x + ROI.width - ImageWidth + 1;
const int OFFSET_Y = ROI.y + ROI.height - ImageHeight + 1;
//qDebug() << "CHECK   ROI: " << ROI.x << ROI.y << ROI.width << ROI.height;

if (OFFSET_X > 0) ROI.x = ImageWidth - ROI.width - 1;
if (OFFSET_Y > 0) ROI.y = ImageHeight - ROI.height - 1;

if (ROI.x < 0) ROI.x = 0;
if (ROI.y < 0) ROI.y = 0;
//qDebug() << "CORRECT ROI: " << ROI.x << ROI.y << ROI.width << ROI.height << " OFFSET_X: " << OFFSET_X << " OFFSET_Y: " << OFFSET_Y ;

}


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

ContoursProcessorClass::ContoursProcessorClass() 
{
    ContourAreas.resize(100);
    ContourRects.resize(100);
    ContourRectsOld.resize(100);
    ContourStatistics.resize(100);
}

void ContoursProcessorClass::SetImage(const cv::Mat& Image)
{
	Contours.clear(); Hierarchy.clear();
  InputImage = Image;

	cv::findContours(Image,Contours,Hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);

	   if(Contours.size() <= 1) return;
     if(Contours.size() > ContourRects.size()) { ContourRects.resize(Contours.size()); ContourAreas.resize(Contours.size());}

     int n = 0;
     int LastContMaxArea = 0;
     int ContourNumber = 0;
     float ContourDistortion = 0;
     cv::Rect ContourRect;

     for(auto& ContInfo: Hierarchy)
     {
      if(ContInfo[3] != -1) { n++; continue; }

      if(cv::contourArea(Contours[n]) < 20) { n++; continue;}

                                ContourRect = cv::boundingRect(Contours[n]);
      ContourDistortion = float(ContourRect.width)/ContourRect.height;

      if(ContourDistortion > 4 || ContourDistortion < 0.3) { n++; continue;};

      ContourRects[n] = cv::boundingRect(Contours[n]);
      ContourAreas[n] = cv::contourArea(Contours[n]);

      if(ContourAreas.back() > LastContMaxArea) { LastContMaxArea = ContourAreas.back(); 
                                                  NumberMaxContour = ContourRects.size()-1; }
      n++;
      ContourNumber++;
     }

     std::sort(ContourRects.begin(), ContourRects.end(), [](cv::Rect& rect1, cv::Rect& rect2) -> bool
     {
        return rect1.area() > rect2.area(); 
     });
}

cv::Rect ContoursProcessorClass::GetMostStableContour()
{
   auto min_element = std::max_element(ContourStatistics.begin(), ContourStatistics.begin() + ContourRects.size()-1, 
                                                                                           [](Statistic& Stat1, Statistic& Stat2) -> bool 
                                                                                           {return Stat1.GetAvarageValue() < Stat2.GetAvarageValue();});
   int number = std::distance(ContourStatistics.begin() , min_element);
    if(number > ContourRects.size()) return ContourRects[0];

   return ContourRects[number];
}

double ContoursProcessorClass::GetMinimumContourDispersion()
{
   auto min_element = std::max_element(ContourStatistics.begin(), ContourStatistics.begin() + ContourRects.size()-1, 
                                                                                           [](Statistic& Stat1, Statistic& Stat2) -> bool 
                                                                                           {return Stat1.GetAvarageValue() < Stat2.GetAvarageValue();});
   return min_element->GetDispersionValue();
}

cv::Rect ContoursProcessorClass::GetMaxContourRect()
{
      if(ContourRects.size() <= NumberMaxContour) return cv::Rect(0,0,30,30);
  return ContourRects[NumberMaxContour];
}

QPair<double,double> ContoursProcessorClass::GetCentroid(const cv::Mat& Image)
{
		double sum_x = 0.0, sum_y = 0.0, sum = 0.0;

		for (int row = 0; row < Image.rows; row++)
		{
			const uint8_t* RowData = Image.ptr<const uint8_t>(row);
			for (int col = 0; col < Image.cols; col++)
			{
					sum += RowData[col];
					sum_x += RowData[col] * col;
					sum_y += RowData[col] * row;
			}
		}

    return QPair<double,double>(sum_x/sum,sum_y/sum);
}

void ContoursProcessorClass::CalcContoursStatistic()
{
    if(ContourStatistics.size() < ContourRects.size()) ContourStatistics.resize(ContourRects.size() + 5);
    double stretchness = 0;

    for(int n = 0; n < ContourRects.size(); n++) 
    {
      auto& rect = ContourRects[n]; 
      stretchness = double(rect.width)/rect.height;
      if(rect.area() < 10*5) continue;
      if(stretchness < 0.2 || stretchness > 4) continue;
      //qDebug() << "AREA: " << rect.area() << "STRETCHNESS: " << stretchness;

      if(isROIValid(rect, InputImage.size().width)) Statistic::Norm(GetCentroid(InputImage(rect))) >> ContourStatistics[n];
    }
}

void ContoursProcessorClass::ClearBlotchRects(cv::Mat& OutputImage)
{
  //            CalcContoursStatistic();
  //auto ObjectRect = GetMostStableContour();
  
  //if(ContourRects[0].area() < 200*200) ObjectRect = ContourRects[0];
  ObjectRect = ContourRects[0];


  cv::Rect blankRoi(cv::Point(0, 0), cv::Point(OutputImage.size().width-1, ObjectRect.y));
  cv::Rect blankRoi2(cv::Point(0, ObjectRect.y + ObjectRect.height-1), cv::Point(ObjectRect.x + ObjectRect.width, OutputImage.size().height-1));

  cv::Rect blankRoi3(cv::Point(0, ObjectRect.y), cv::Point(ObjectRect.x, ObjectRect.y+ObjectRect.height));
  cv::Rect blankRoi4(cv::Point(ObjectRect.x + ObjectRect.width-1, ObjectRect.y), cv::Point(OutputImage.size().width-1, OutputImage.size().height-1));

  OutputImage(blankRoi).setTo(cv::Scalar(0,0,0));
  OutputImage(blankRoi2).setTo(cv::Scalar(0,0,0));
  OutputImage(blankRoi3).setTo(cv::Scalar(0,0,0));
  OutputImage(blankRoi4).setTo(cv::Scalar(0,0,0));

}

void ContoursProcessorClass::PrintContoursRect(cv::Mat& OutputImage)
{
	 if(ContourRects.empty()) return;

   int Counter = 0;
   for(auto Rect: ContourRects) 
   {
     if(Counter == 0) continue;
     CheckROI(Rect, OutputImage.cols-1);
     cv::rectangle(OutputImage,Rect,cv::Scalar(600,600,600),1);

     Counter++; if(Counter == 5) return;
   }

}

bool ContoursProcessorClass::isROIValid(const cv::Rect& ROI, int SizeImage)
{
	int OFFSET_X = ROI.x + ROI.width - SizeImage;
	int OFFSET_Y = ROI.y + ROI.height - SizeImage;

	if (OFFSET_X > 0 || OFFSET_Y > 0)  return false;
	if (ROI.x    < 1 || ROI.y    < 1)  return false;
  if (ROI.width*ROI.height == 0) return false;

    return true;
}

void ContoursProcessorClass::CheckROI(cv::Rect& ROI, int SizeImage)
{
	if(isROIValid(ROI, SizeImage)) return;

	int OFFSET_X = ROI.x + ROI.width - SizeImage  + 1;
	int OFFSET_Y = ROI.y + ROI.height - SizeImage + 1;

	if (OFFSET_X > 0) ROI.width -= OFFSET_X;
	if (OFFSET_Y > 0) ROI.height -= OFFSET_Y;

	if (ROI.x < 1) ROI.x = 1;
	if (ROI.y < 1) ROI.y = 1;

}

void FilterBlotchClass::FilterImage(cv::Mat& FilteringImage, cv::Mat& RawImage)
{
	try 
	{ 
    FindContours(RawImage); 
		FindContours.ClearBlotchRects(FilteringImage); 
    //FindContours.PrintContoursRect(FilteringImage);
	}
	catch (const cv::Exception& cv_ec) 
	{ 
		if(cv_ec.code == cv::Error::StsAssert) { qDebug() << Qt::endl << TAG_NAME << " ASSREST FAILED OPENCV"; return;}
		if(cv_ec.code == cv::Error::BadROISize) { qDebug() << Qt::endl << TAG_NAME << " BAD ROI OPENCV"; return;}
          
		qDebug() << Qt::endl << TAG_NAME << cv_ec.what() << cv_ec.code;	
	}

}

void ModuleImageProcessing::SetProcessingRegim(int Regim)       { }
void ModuleImageProcessing::StartStopProcessing(bool StartStop) { }
void ModuleImageProcessing::SetThreshold(int Thresh)  {qDebug() << "THRES: " << Thresh; this->Threshold = Thresh; }



void ImageTrackerCentroid::CalcCentroid(cv::Mat& Image)
{
		double sum_x = 0.0, sum_y = 0.0, sum = 0.0;

		for (int row = 0; row < Image.rows; row++)
		{
			uint8_t* RowData = Image.ptr<uint8_t>(row);
			for (int col = 0; col < Image.cols; col++)
			{
				  if (RowData[col] < Threshold) { RowData[col] = 0; continue; } RowData[col] = 255;

					sum += RowData[col];
					sum_x += RowData[col] * col;
					sum_y += RowData[col] * row;
			}
		}

		if (sum == 0.0) return;

    CoordsObject[0].first  = sum_x / sum; 
    CoordsObject[0].second = sum_y / sum;
}

QPair<double,double> ImageTrackerCentroid::GetCentroid(cv::Mat& Image)
{
		double sum_x = 0.0, sum_y = 0.0, sum = 0.0;

		for (int row = 0; row < Image.rows; row++)
		{
			uint8_t* RowData = Image.ptr<uint8_t>(row);
			for (int col = 0; col < Image.cols; col++)
			{
					sum += RowData[col];
					sum_x += RowData[col] * col;
					sum_y += RowData[col] * row;
			}
		}

    return QPair<double,double>(sum_x/sum,sum_y/sum);
}


void ImageTrackerCentroid::SlotProcessImage(const cv::Mat& Image)
{
                 ImageProcessing = Image.clone(); 
    ImageInput = ImageProcessing;


    CalcCentroid(ImageInput); 

    emit ImageSourceInterface::SignalNewImage();

    if(ReceiverLinkCoord) PassTwoCoordClass::PassCoord();
}

void ImageTrackerCentroid::TrackObjectCentroid(cv::Mat& Image)
{
             CheckCorrectROI(RectsObject[0]);
  ImageProcessingROI = Image(RectsObject[0]);
  //FilterBlotch.FilterImage(ImageProcessingROI, ImageProcessingROI); 

  CalcCentroid(ImageProcessingROI); 
  CoordsObject[0].first += RectsObject[0].x; 
  CoordsObject[0].second += RectsObject[0].y; 

  RectsObject[0] = cv::Rect(CoordsObject[0].first - 50, CoordsObject[0].second - 50 ,100,100);
}

void ImageTrackerCentroid::FindObjectCentroid(cv::Mat& Image)
{
  FilterErosion(Image, ImageInputTemp); FilterSharpen(ImageInputTemp, Image);
  //FilterErosion(Image,Image);
  //FilterErosion(Image, ImageInputTemp); FilterErosion(ImageInputTemp, Image);
  //FilterSharpen(Image, ImageInputTemp); FilterErosion(ImageInputTemp, Image);
  //FilterSharpen(Image, ImageInputTemp); FilterSharpen(ImageInputTemp, Image);

  Image >> ThresholdAdjuster >> Threshold;

  cv::medianBlur(Image,ImageInputTemp,5);
  cv::threshold(Image,Image,Threshold,256,cv::THRESH_BINARY);

  FilterBlotch.FilterImage(Image, Image); 

  CalcCentroid(ImageInput); 

   RectsObject[0] = cv::Rect(CoordsObject[0].first - 50, CoordsObject[0].second - 50 ,100,100);
  CoordsObject[0] >> StatisticCoord; 

  if(StatisticCoord.IsCoordLoaded()) 
     StatisticCoord.GetDispersionNorm() >> Saturation(20) 
                                        >> InversionBinary 
                                        >> StatisticDispersion;

                                        if(StatisticDispersion.IsValueLoaded()) 
                                           StatisticDispersion.GetAvarageValue() >> TrackingDetector(0.9);
}

void ImageTrackerCentroid::CalcThreshold() 
{
  //if(OptimizationLumin.isEnabled())
  //{
  //ImageProcessing >> OptimizationLumin >> Threshold;
  //                                        Threshold >> StatisticThreshold;
  //                   OptimizationLumin.GetValue() >> OptimizationDisp;

  //if(StatisticThreshold.GetDispersionValue() < 1) OptimizationLumin.Disable();
  //return;
  //}

  //ImageProcessing >> OptimizationDisp >> Threshold;
}

void ImageTrackerCentroid::SlotProcessImage()
{

  std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint = std::chrono::high_resolution_clock::now();

  MutexImageAccess.lock();
  ImageInput = SourceImage->GetImageToProcess().clone(); 

  if(ImageInputTemp.size().width != ImageInput.size().width || 
     ImageInputTemp.size().height != ImageInput.size().height)
     ImageInputTemp = ImageInput.clone();

  ImageProcessing = ImageInput;

  //CalcThreshold();
  //FindObjectCentroid(ImageProcessing);  

  if(TrackingDetector.isSignal()) TrackObjectCentroid(ImageProcessing); 
  else                             FindObjectCentroid(ImageProcessing);  

  MutexImageAccess.unlock();
  emit ImageSourceInterface::SignalNewImage();

  if(TrackingDetector.isSignal()) *this >> *Link;

  std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint2 = std::chrono::high_resolution_clock::now();
  //qDebug() << Filter10 << "PROCESS PERIOD: " << std::chrono::duration<double>((TimePoint2 - TimePoint)).count()*1000;
  qDebug() << Filter10 << "IMAGE PROC OUT: " << GetOutput().first << GetOutput().second;
}
  //qDebug() << Filter100 << " [ COORD ] " << CoordsObject[0].first << CoordsObject[0].second
  //                      << " [ ROI   ] " << RectsObject[0].x      << RectsObject[0].y;

  //ProcessTimePoint = std::chrono::high_resolution_clock::now();
  //auto Dur = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ProcessTimePoint);
  //qDebug() << "PROCESS DURATION - " << Dur.count();

  //if(ContourRects.empty())return;
  //   CheckROI(ContourRects[0],OutputImage.size().width-1);

  //if(MaskImage.size().width != OutputImage.size().width || MaskImage.size().height != OutputImage.size().height)
  //{
  //   MaskImage = OutputImage.clone(); MaskImage.setTo(cv::Scalar(0,0,0));
  //   //MaskImage = cv::Mat::zeros(OutputImage.rows,OutputImage.cols,OutputImage.type());
  //}

  //cv::rectangle(MaskImage,ContourRects[0],cv::Scalar(1000,1000,1000), -1);
  //cv::bitwise_and(OutputImage, OutputImage, OutputImage,MaskImage);
  //MaskImage.setTo(cv::Scalar(0,0,0));