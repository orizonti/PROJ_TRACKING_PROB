#include "contour_processing.h"

#include <chrono>
#include <opencv2/opencv.hpp>
#include <qnamespace.h>
#include <debug_output_filter.h>


ContoursProcessorClass::ContoursProcessorClass() 
{
    ContourAreas.resize(10);
    ContourRects.resize(10);
    RectOutput = ContourRects.begin();
    RectEnd = ContourRects.end();

    std::fill(ContourRects.begin(), ContourRects.end(), cv::Rect(0,0,0,0));
}

void ContoursProcessorClass::SetImage(const cv::Mat& Image)
{
	Contours.clear(); 
   rectImage.width = Image.cols;
   rectImage.height = Image.rows;

	cv::findContours(Image,Contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

	  if(Contours.size() <= 0) return;

        float ContourArea = 0;
     cv::Rect ContourRect;
              ContoursCount = 0;

     auto AreaOutput = ContourAreas.begin();
          RectOutput = ContourRects.begin();
             RectEnd = ContourRects.begin(); 

      int ContourMaxArea = 0;
      for(auto& contour: Contours)
      {
         ContourArea = cv::contourArea (contour);
         ContourRect = cv::boundingRect(contour);

         if(ContourArea < minLimit || ContourArea > maxLimit) continue; 

         if(ContourArea > ContourMaxArea) { ContourMaxArea = ContourArea; rectMax = ContourRect; }
         *AreaOutput = ContourArea;
         *RectOutput = ContourRect;

         AreaOutput++; RectOutput++; ContoursCount++; if(RectOutput == ContourRects.end()) return;
      }
      CheckCorrectROI(rectMax);
     
             RectEnd = RectOutput; RectOutput = ContourRects.begin(); //if(countContoursFiltered() > 2) qDebug() << "COUNT: " << countContoursFiltered();
     RectEndFiltered = RectOutputFiltered;
}

int ContoursProcessorClass::countContours() { return RectEnd - ContourRects.begin(); }


                 cv::Rect ContoursProcessorClass::getMaxRect() { return rectMax; }
std::pair<float,float> ContoursProcessorClass::getPosMaxRect() { return std::make_pair(rectMax.x + rectMax.width/2, rectMax.y + rectMax.height/2); }


void ContoursProcessorClass::PrintContoursRect(cv::Mat& OutputImage)
{
	if(ContourRects.empty()) return;
   int counter = 0;
   for(auto Rect: ContourRects) {if(counter >= ContoursCount) return; cv::rectangle(OutputImage,Rect,cv::Scalar(600,600,600),1); counter++; }
}

cv::Rect& ContoursProcessorClass::getScaledMaxRect(float Scale)
{
   rectMaxScaled.width  = rectMax.width*Scale;
   rectMaxScaled.height = rectMax.height*Scale;
   rectMaxScaled.x = rectMax.x - (rectMaxScaled.width - rectMax.width)/2;
   rectMaxScaled.y = rectMax.y - (rectMaxScaled.height - rectMax.height)/2;

   CheckCorrectROI(rectMaxScaled);
            return rectMaxScaled;
};

std::vector<cv::Rect>& ContoursProcessorClass::getMaxOutsideRects()
{
                        rectObject = getScaledMaxRect(1.1);
   auto rect_y_bottom = rectObject.y + rectObject.height;
   auto rect_x_right  = rectObject.x + rectObject.width;

   rectsMaxOutside[0]  = cv::Rect(cv::Point(1,1)                   , cv::Point(rectImage.width,rectObject.y+1));
   rectsMaxOutside[1]  = cv::Rect(cv::Point(1,rect_y_bottom)       , cv::Point(rectImage.width,rectImage.height-1));

   rectsMaxOutside[2]  = cv::Rect(cv::Point(1           ,rectObject.y), cv::Point(rectObject.x+1   ,rect_y_bottom));
   rectsMaxOutside[3]  = cv::Rect(cv::Point(rect_x_right,rectObject.y), cv::Point(rectImage.width-1,rect_y_bottom));


   auto& Rects = rectsMaxOutside;
   //qDebug() << "[ RECT MAX ]" <<  rectMax.x << rectMax.y << rectMax.width << rectMax.height 
   //         << "[ IMAGE ]"    <<  rectImage.width << rectImage.height << "[COUNT]" << ContoursCount;

//		 qDebug() << "FILTER RECTS: " << Rects[0].x << Rects[0].y << Rects[0].width << Rects[0].height << "|"
//                                   << Rects[1].x << Rects[1].y << Rects[1].width << Rects[1].height << "|"
//                                    << Rects[2].x << Rects[2].y << Rects[2].width << Rects[2].height << "|"
//                                    << Rects[3].x << Rects[3].y << Rects[3].width << Rects[3].height
//                                    << "[ MAX ]" <<  rectMax.x << rectMax.y << rectMax.width << rectMax.height;

   return rectsMaxOutside;
}



void ContoursProcessorClass::CheckCorrectROI(cv::Rect& ROI)
{
const int ImageWidth  = rectImage.width;
const int ImageHeight = rectImage.height;
if(ROI.width  > ImageWidth) ROI.width = ImageWidth/2;
if(ROI.height > ImageHeight) ROI.height = ImageHeight/2;

auto OFFSET_X = (ROI.x + ROI.width ) - ImageWidth;  if(OFFSET_X < 0) OFFSET_X = 0;
auto OFFSET_Y = (ROI.y + ROI.height) - ImageHeight; if(OFFSET_Y < 0) OFFSET_Y = 0;

   ROI.x -= OFFSET_X;
   ROI.y -= OFFSET_Y;

if(ROI.x <= 0 ) ROI.x = 1;
if(ROI.y <= 0 ) ROI.y = 1;

}
