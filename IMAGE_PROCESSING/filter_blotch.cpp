#include "filter_blotch.h"
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>


void FilterBlotchClass::FilterImage(cv::Mat& Image)
{
	if(Image.empty()) return;

	try 
	{ 
         cv::minMaxLoc(Image, &MinPixel, &MaxPixel);
		 Threshold = MinPixel + (MaxPixel - MinPixel)*0.5;
		 cv::threshold(Image,Image,Threshold,255,cv::THRESH_BINARY);

                    FindContours(Image); 
	     auto Rects = FindContours.getMaxOutsideRects(); 
    
		 for(auto& Rect: Rects) { Image(Rect).setTo(cv::Scalar(0)); }
	}

	catch (const cv::Exception& cv_ec) 
	{ 
		if(cv_ec.code == cv::Error::StsAssert) { std::cout << TAG_NAME << "[ STS ASSERT ] " << cv_ec.msg << std::endl; return;}
		if(cv_ec.code == cv::Error::BadROISize) { std::cout << TAG_NAME << "[ BAD ROI ] " << cv_ec.msg << std::endl; return;}
          
    std::cout << TAG_NAME << cv_ec.what() << cv_ec.code;	
	}

}
