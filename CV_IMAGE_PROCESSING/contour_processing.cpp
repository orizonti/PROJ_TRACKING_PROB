#include "contour_processing.h"

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

      if(cv::contourArea(Contours[n]) < 3) { n++; continue;}
      if(cv::contourArea(Contours[n]) > 10*10) { n++; continue;}

                                ContourRect = cv::boundingRect(Contours[n]);
      ContourDistortion = float(ContourRect.width)/ContourRect.height;

      if(ContourDistortion > 2.1 || ContourDistortion < 0.5) { n++; continue;};

      ContourRects[n] = cv::boundingRect(Contours[n]);
      ContourAreas[n] = cv::contourArea(Contours[n]);

      if(ContourAreas.back() > LastContMaxArea) { LastContMaxArea = ContourAreas.back(); 
                                                  NumberMaxContour = ContourRects.size()-1; }
      n++;
      ContourNumber++;
     }


        double maxVal1; 
        double maxVal2; 

     std::sort(ContourRects.begin(), ContourRects.end(), [Image, &maxVal1, &maxVal2](cv::Rect& rect1, cv::Rect& rect2) -> bool
     {
        double minVal; cv::Point minLoc, maxLoc; 
        cv::minMaxLoc(Image(rect1), &minVal, &maxVal1, &minLoc, &maxLoc);
        cv::minMaxLoc(Image(rect2), &minVal, &maxVal2, &minLoc, &maxLoc);
        return maxVal1 > maxVal2; 
     });

     //std::sort(ContourRects.begin(), ContourRects.end(), [](cv::Rect& rect1, cv::Rect& rect2) -> bool
     //{
     //   return rect1.area() > rect2.area(); 
     //});
}

cv::Rect ContoursProcessorClass::GetMostStableContour()
{
   auto min_element = std::max_element(ContourStatistics.begin(), ContourStatistics.begin() + ContourRects.size()-1, 
                                     [](StatisticNode<double>& Stat1, StatisticNode<double>& Stat2) -> bool 
                                     {return Stat1.GetAvarageValue() < Stat2.GetAvarageValue();});

   int number = std::distance(ContourStatistics.begin() , min_element);
    if(number > ContourRects.size()) return ContourRects[0];

   return ContourRects[number];
}

double ContoursProcessorClass::GetMinimumContourDispersion()
{
   auto min_element = std::max_element(ContourStatistics.begin(), ContourStatistics.begin() + ContourRects.size()-1, 
                                       [](StatisticNode<double>& Stat1, StatisticNode<double>& Stat2) -> bool 
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

      if(isROIValid(rect, InputImage.size().width)) StatisticCoord<double>::Norm(GetCentroid(InputImage(rect))) >> ContourStatistics[n];
    }
}

void ContoursProcessorClass::ClearBlotchRects(cv::Mat& OutputImage)
{
  ObjectRect = ContourRects[0];

  cv::Rect blankRoi(cv::Point(0, 0), cv::Point(OutputImage.size().width-1, ObjectRect.y));
  cv::Rect blankRoi2(cv::Point(0, ObjectRect.y + ObjectRect.height-1), cv::Point(ObjectRect.x + ObjectRect.width, OutputImage.size().height-1));

  cv::Rect blankRoi3(cv::Point(0, ObjectRect.y), cv::Point(ObjectRect.x, ObjectRect.y+ObjectRect.height));
  cv::Rect blankRoi4(cv::Point(ObjectRect.x + ObjectRect.width-1, ObjectRect.y), cv::Point(OutputImage.size().width-1, OutputImage.size().height-1));

  CheckROI(blankRoi, OutputImage.size().width);
  CheckROI(blankRoi2, OutputImage.size().width);
  CheckROI(blankRoi3, OutputImage.size().width);
  CheckROI(blankRoi4, OutputImage.size().width);

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
	int OFFSET_X = ROI.x + ROI.width - SizeImage + 2;
	int OFFSET_Y = ROI.y + ROI.height - SizeImage + 2;

	if (OFFSET_X > 0 || OFFSET_Y > 0)  return false;
	if (ROI.x    < 1 || ROI.y    < 1)  return false;
  if (ROI.width*ROI.height == 0) return false;

  return true;
}

void ContoursProcessorClass::CheckROI(cv::Rect& ROI, int SizeImage)
{
	int OFFSET_X = ROI.x + ROI.width - SizeImage  + 3;
	int OFFSET_Y = ROI.y + ROI.height - SizeImage + 3;

	if (OFFSET_X > 0) ROI.width -= OFFSET_X;  if(ROI.width <= 0) ROI.width = 2;
	if (OFFSET_Y > 0) ROI.height -= OFFSET_Y; if(ROI.height <= 0) ROI.height = 2;

	if (ROI.x < 0) ROI.x = 0;
	if (ROI.y < 0) ROI.y = 0;
}
