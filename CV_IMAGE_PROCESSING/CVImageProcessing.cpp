#include "CVImageProcessing.h"
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <qnamespace.h>
#include <chrono>

bool operator==(std::pair<int,int> pair, std::pair<int,int> pair2) { return pair.first == pair2.first && pair.second == pair2.second; };
bool operator==(std::pair<int,int> pair, int value) { return pair.first == value && pair.second == value; };
bool operator!=(std::pair<int,int> pair, std::pair<int,int> pair2) { return pair.first != pair2.first && pair.second != pair2.second; };
bool operator!=(std::pair<int,int> pair, int value) { return pair.first != value && pair.second != value; };

CVImageProcessing::CVImageProcessing(QObject* parent) : ImageSourceInterface(parent)
{
  qRegisterMetaType<const cv::Mat&>();
  //if(QFile::exists("D:/DATA/FindingTemplate.png"))
  //ImageTemplate = imread("D:/DATA/FindingTemplate.png",cv::IMREAD_GRAYSCALE);

  if(QFile::exists("/home/broms/DATA/FindingTemplate.png"))
  ImageTemplate = imread("/home/broms/DATA/FindingTemplate.png",cv::IMREAD_GRAYSCALE);

  TemplateRect = cv::Size(ImageTemplate.cols,ImageTemplate.rows);
  
  qDebug() << "LOAD TEMPLATE: SIZE: " << TemplateRect.width << TemplateRect.height;

  Trajectory.resize(30,std::make_pair(0,0));
  TrajectoryIncrement.resize(30,std::make_pair(0,0));
  TrajectoryIncrementAvarage.resize(30,std::make_pair(0,0));
  std::cout << cv::getBuildInformation() << std::endl;

  QObject::connect(&timerDisplay,SIGNAL(timeout()),this, SLOT(SlotDisplayProcessingImage()));
}

CVImageProcessing::~CVImageProcessing()
{
 qDebug() << "STOP AND DEINIT CAMEA";
}

void CVImageProcessing::SlotSetImageToProcess(const cv::Mat& Image)
{
    ImageInput = Image;
    this->SlotProcessImage();
}

void CVImageProcessing::FindImageCentroid(cv::Mat& Image)
{
    cv::threshold(Image,ImageProcessing,80,256,cv::THRESH_BINARY);
		double sum_x = 0.0, sum_y = 0.0, sum = 0.0;

		for (short row = 0; row < this->ImageProcessing.rows; row++)
		{
			const unsigned short* RowData = this->ImageProcessing.ptr<unsigned short>(row);
			for (short col = 0; col < this->ImageProcessing.cols; col++)
			{
				//if (RowData[col] > 100)
				//{
					sum += RowData[col];
					sum_x += RowData[col] * col;
					sum_y += RowData[col] * row;
				//}
			}
		}

		//if (sum == 0.0) return;

    this->centroid_abs.first = sum_y / sum; // get spot center coord
    this->centroid_abs.second = sum_x / sum;

    ImageToDisplay = QImage(ImageProcessing.data,
                            ImageProcessing.cols,
                            ImageProcessing.rows,
           static_cast<int>(ImageProcessing.step),
              QImage::Format_Grayscale8 );

             SignalNewImage(ImageToDisplay);
}

void CVImageProcessing::FindImageTemplate(cv::Mat& Image)
{
  if(ImageTemplate.empty()) qDebug() << "[IMG PROC]: TEMPLATE EMPTY";

  ProcessTimePoint = std::chrono::high_resolution_clock::now();

  int result_cols = Image.cols - ImageTemplate.cols + 1;
  int result_rows = Image.rows - ImageTemplate.rows + 1;
  

  ProcessResult.create( result_rows, result_cols, CV_32FC1 );

  //===========================
                                ImageInput = Image; 
                                ImageProcessing = Image.clone();
  if(Trajectory.front() != 0) { ImageProcessing = ImageProcessing(ROIFindRect); FLAG_FAST_MATCHING = true; }
  //===========================

  //ContoursProcessor.SetImage(ImageProcessing);
  //ContoursProcessor.PrintContoursRect(ImageToDisplay); 
  //ContoursProcessor.ClearBlotchRects(ImageProcessing); 

  FilterBlotch.FilterImage(ImageProcessing, ImageProcessing);
  FLAG_DISPLAY_IMAGE = true;

  cv::matchTemplate( ImageProcessing, ImageTemplate, ProcessResult, cv::TM_SQDIFF); 

  cv::normalize( ProcessResult, ProcessResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
  double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;

  cv::minMaxLoc( ProcessResult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

  if(FLAG_FAST_MATCHING) { MatchObjectPos = minLoc + cv::Point(ROIFindRect.x, ROIFindRect.y); }
  else MatchObjectPos = minLoc;


  auto Dur = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ProcessTimePoint);

  TrajectoryIncrement.push_back(std::make_pair<float,float>(float(MatchObjectPos.x - Trajectory.back().first),
                                               float(MatchObjectPos.y - Trajectory.back().second)));
  auto Vel = TrajectoryIncrement.back();
 
  AvarageVel = std::accumulate(TrajectoryIncrement.end() - 10, 
                                 TrajectoryIncrement.end(),
                                 std::make_pair<float,float>(0,0),
                                 []( std::pair<float,float> acc, const std::pair<float,float>& val) mutable -> std::pair<float,float>
                                                                                      { acc.first += val.first/10.0;
                                                                                        acc.second += val.second/10.0;
                                                                                      return acc;}
                                 );
  qDebug() << "PROCESS DURATION - " << Dur.count();

  ROIFindRect = cv::Rect(MatchObjectPos.x + AvarageVel.first - 10, MatchObjectPos.y + AvarageVel.second - 20, 80, 80);

  if(!IsROIValid(ROIFindRect)) CheckCorrectROI(ROIFindRect);

  Trajectory.erase(Trajectory.begin());
  Trajectory.push_back(std::make_pair(MatchObjectPos.x,MatchObjectPos.y));

  TrajectoryIncrement.erase(TrajectoryIncrement.begin());
  //TrajectoryIncrementAvarage.push_back(AvarageVel);
  //TrajectoryIncrementAvarage.erase(TrajectoryIncrementAvarage.begin());

  emit SignalObjectDetected(MatchObjectPos,AvarageVel,ROIFindRect);
}

void CVImageProcessing::SlotProcessImage()
{
  //FindImageTemplate(ImageInput);
  FindImageCentroid(ImageInput);
}


void CVImageProcessing::LinkToImageSource(ImageSourceInterface* ImageSource)
{
   QObject::connect(ImageSource,SIGNAL(SignalNewImage(const cv::Mat&)),this,SLOT(SlotSetImageToProcess(const cv::Mat&)),Qt::QueuedConnection);
   //QObject::connect(ImageSource,SIGNAL(SignalNewImage(const cv::Mat&)),this,SLOT(SlotSetImageToProcess(cv::Mat&)));
   timerDisplay.start(50);
}

void CVImageProcessing::CalcVelocity() { }

bool CVImageProcessing::IsROIValid(cv::Rect& ROI)
{
const int& ImageWidth = ImageInput.cols;
const int& ImageHeight = ImageInput.rows;

if((ROI.x + ROI.width + 1) > ImageWidth) return false;
if((ROI.y + ROI.height + 1) > ImageHeight) return false;
if (ROI.x < 0) return false;
if (ROI.y < 0) return false;

return true;
}

void CVImageProcessing::CheckCorrectROI(cv::Rect& ROI)
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




void CVImageProcessing::SlotDisplayProcessingImage()
{
   //if(FLAG_DISPLAY_IMAGE)
   //cv::imshow("PROCESSING IMAGE",ImageProcessing);
   //cv::imshow("PROCESSING IMAGE",ImageToDisplay);

   //FLAG_DISPLAY_IMAGE = false;
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
     //                              << "PARENT:" << ContInfo[3];
     //qDebug() << "=======================================";
//=====================================

void FindContoursProcessor::SetImage(const cv::Mat& Image)
{
	Contours.clear(); Hierarchy.clear();

	cv::findContours(Image,Contours,Hierarchy,cv::RETR_TREE,cv::CHAIN_APPROX_SIMPLE);

     ContourRects.clear();
     ContourAreas.clear();
	 if(Contours.size() <= 1) return;

     int n = 0;
     int LastContMaxArea = 0;
     for(auto& ContInfo: Hierarchy)
     {
      if(ContInfo[3] != -1) { n++; continue; }
      ContourRects.push_back(cv::boundingRect(Contours[n]));
      ContourAreas.push_back(cv::contourArea(Contours[n]));

      if(ContourAreas.back() > LastContMaxArea) { LastContMaxArea = ContourAreas.back(); 
                                                  NumberMaxContour = ContourRects.size()-1; }
      n++;
     }
}

void FindContoursProcessor::ClearBlotchRects(cv::Mat& OutputImage)
{
	if(Contours.size() <= 1) return;
    for(int n = 0; n < ContourRects.size(); n++) 
    {
      if(n == NumberMaxContour) continue;

      auto& rect = ContourRects[n]; CheckROI(rect, OutputImage.cols-1);

      OutputImage(rect) = cv::Scalar(0,0,0);
    }
}
void FindContoursProcessor::PrintContoursRect(cv::Mat& OutputImage)
{
	if(Contours.size() <= 1) return;
    for(int n = 0; n < ContourRects.size(); n++) 
    {
      if(n == NumberMaxContour) continue;

      auto& rect = ContourRects[n]*1.2; CheckROI(rect, OutputImage.cols-1);

      //cv::ellipse(OutputImage,cv::RotatedRect(cv::Point2f(rect.x + rect.width/2, rect.y + rect.height/2),
      //										cv::Size2f(rect.width, rect.height), 0.0),
      //										cv::Scalar(600,600,600),1);

      cv::rectangle(OutputImage,rect,cv::Scalar(600,600,600),1);
    }

}

bool FindContoursProcessor::isROIValid(const cv::Rect& ROI, int ImageSize)
{
	int OFFSET_X = ROI.x + ROI.width - 255;
	int OFFSET_Y = ROI.y + ROI.height - 255;

	if (OFFSET_X > 0 || OFFSET_Y > 0)  return false;
	if (ROI.x    < 1 || ROI.y    < 1)  return false;

    return true;
}

void FindContoursProcessor::CheckROI(cv::Rect& ROI, int ImageSize)
{
	if(isROIValid(ROI, ImageSize)) return;

	int OFFSET_X = ROI.x + ROI.width - ImageSize;
	int OFFSET_Y = ROI.y + ROI.height - ImageSize;

	if (OFFSET_X > 0) ROI.x -= OFFSET_X;
	if (OFFSET_Y > 0) ROI.y -= OFFSET_Y;

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
		if(cv_ec.code == cv::Error::StsAssert) { qDebug() << Qt::endl << TAG << " ASSREST FAILED OPENCV"; return;}
		if(cv_ec.code == cv::Error::BadROISize) { qDebug() << Qt::endl << TAG << " BAD ROI"; return;}
          
		qDebug() << Qt::endl << TAG << cv_ec.what() << cv_ec.code;	
	}

}

QImage CVImageProcessing::GetNewImageDisplay()
{
			ImageToDisplay = QImage(ImageProcessing.data,
										ImageProcessing.cols,
										ImageProcessing.rows,
						static_cast<int>(ImageProcessing.step),
								QImage::Format_Grayscale8 );
                return ImageToDisplay;
}
cv::Mat& CVImageProcessing::GetNewImageProcess()
{
  return ImageProcessing;
}

//cv::Rect& operator*(float Scale, cv::Rect& rect)
//{
//return rect*Scale;
//}
//cv::Rect& operator*(cv::Rect& rect, float Scale)
//{
//	rect.x -= (rect.width*Scale - rect.width)/2;
//	rect.y -= (rect.height*Scale - rect.height)/2;
//	rect.width *= Scale;
//	rect.height *= Scale;
//	return rect;
//}

//sum >> StatProcessor;
//if(StatProcessor.IsValueLoaded() && sum < StatProcessor.AvarageValue/10) return;
//this->spot_coord_abs >> StatProcessor;

//this->x_pos_spot_in_strob = x_pos_spot_in_roi + roi_rect_in_strob.x;
//this->y_pos_spot_in_strob = y_pos_spot_in_roi + roi_rect_in_strob.y;
//this->centroid_abs.first = x_pos_spot_in_strob + strob_coord.first;
//this->centroid_abs.second = y_pos_spot_in_strob + strob_coord.second;

//this->roi_rect_in_strob.x = x_pos_spot_in_strob - roi_rect_in_strob.width / 2;
//this->roi_rect_in_strob.y = y_pos_spot_in_strob - roi_rect_in_strob.height / 2;
//CheckROIPos(roi_rect_in_strob);
//====================================================================
		//cv::Mat ROIImage = StrobImage(this->roi_rect_in_strob);
		//if (this->FlagThresholdAutoControl) Threshold = CalcThresholdlByLuminosity(ROIImage);
		//ROIImageFiltered = ROIImage;
		//cv::medianBlur(ROIImage, this->ROIImageFiltered, 5);