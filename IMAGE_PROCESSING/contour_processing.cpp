#include "contour_processing.h"

#include <qnamespace.h>
#include <debug_output_filter.h>


ContoursProcessorClass::ContoursProcessorClass() 
{
    ContourAreas.resize(10);
    ContourRects.resize(10);
    ContourRectsFiltered.resize(10);
    RectOutput = ContourRects.begin();
    RectEnd = ContourRects.end();

    std::fill(ContourRects.begin(), ContourRects.end(), cv::Rect(0,0,0,0));
    std::fill(ContourRectsFiltered.begin(), ContourRectsFiltered.end(), cv::Rect(0,0,0,0));
}

void ContoursProcessorClass::SetImage(const cv::Mat& Image)
{
	Contours.clear(); 

	cv::findContours(Image,Contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

	  if(Contours.size() <= 0) return;

        float ContourArea = 0;
     cv::Rect ContourRect;
              ContoursCount = 0;

     auto AreaOutput = ContourAreas.begin();
          RectOutput = ContourRects.begin();
             RectEnd = ContourRects.begin(); 

          RectOutputFiltered = ContourRectsFiltered.begin();
             RectEndFiltered = ContourRectsFiltered.begin();

      int ContourMaxArea = 0;
      //qDebug() << "CONTOURS: " << Contours.size();
      for(auto& contour: Contours)
      {
         ContourArea = cv::contourArea (contour);
         ContourRect = cv::boundingRect(contour);

         if(ContourArea < minLimit || ContourArea > maxLimit) 
         {
         *RectOutputFiltered = ContourRect; 
          RectOutputFiltered++; if(RectOutputFiltered == ContourRectsFiltered.end()) RectOutputFiltered = ContourRectsFiltered.begin();
          continue; 
         }

         if(ContourArea > ContourMaxArea) { ContourMaxArea = ContourArea; rectMax = ContourRect; }
         *AreaOutput = ContourArea;
         *RectOutput = ContourRect;

         AreaOutput++; RectOutput++; ContoursCount++; if(RectOutput == ContourRects.end()) return;
      }
     
             RectEnd = RectOutput; RectOutput = ContourRects.begin(); //if(countContoursFiltered() > 2) qDebug() << "COUNT: " << countContoursFiltered();
     RectEndFiltered = RectOutputFiltered;
}

int ContoursProcessorClass::countContours() { return RectEnd - ContourRects.begin(); }
int ContoursProcessorClass::countContoursFiltered() { return RectEndFiltered - ContourRectsFiltered.begin(); }



                 cv::Rect ContoursProcessorClass::getMaxRect() { return rectMax; }
std::pair<float,float> ContoursProcessorClass::getPosMaxRect() { return std::make_pair(rectMax.x + rectMax.width/2, rectMax.y + rectMax.height/2); }

cv::Rect ContoursProcessorClass::getRect(int num) 
{  if(num >= ContourRects.size() ) return ContourRects[0]; 
                                   return ContourRects[num]; };

std::pair<float,float> ContoursProcessorClass::getPosRect(int num)
{  
  if(num >= ContourRects.size() )  return std::make_pair(0,0);

  return std::make_pair(ContourRects[num].x+ContourRects[num].width/2, 
                        ContourRects[num].y+ContourRects[num].height/2); };

void ContoursProcessorClass::PrintContoursRect(cv::Mat& OutputImage)
{
	if(ContourRects.empty()) return;
   int counter = 0;
   for(auto Rect: ContourRects) {if(counter >= ContoursCount) return; cv::rectangle(OutputImage,Rect,cv::Scalar(600,600,600),1); counter++; }

}


//std::sort(ContourRects.begin(), ContourRects.end(), [](cv::Rect& rect1, cv::Rect& rect2) -> bool
//{
//   return rect1.area() > rect2.area(); 
//});