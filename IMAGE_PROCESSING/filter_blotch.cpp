#include "filter_blotch.h"
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>

bool FilterBlotchClass::isEmpty(const cv::Rect& rect)
{
   return rect.width <= 0 || rect.height <= 0 || rect.x <= 0 || rect.y <= 0;
}

void FilterBlotchClass::FilterImage(cv::Mat& Image)
{
	if(Image.empty()) return;

	try 
	{ 
                   FindContours(Image); 
	     auto it = FindContours.beginFiltered();
	 auto it_end = FindContours.endFiltered();
	auto rectMax = FindContours.getMaxRect();

    while(true)	{ if(it == it_end) break; Image(*it).setTo(cv::Scalar(0));  it++; } }
	catch (const cv::Exception& cv_ec) 
	{ 
		if(cv_ec.code == cv::Error::StsAssert) { std::cout << TAG_NAME << "[ STS ASSERT ] " << cv_ec.msg << std::endl; return;}
		if(cv_ec.code == cv::Error::BadROISize) { std::cout << TAG_NAME << "[ BAD ROI ] " << cv_ec.msg << std::endl; return;}
          
    std::cout << TAG_NAME << cv_ec.what() << cv_ec.code;	
	}

}
