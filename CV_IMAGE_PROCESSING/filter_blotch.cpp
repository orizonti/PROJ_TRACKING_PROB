#include "filter_blotch.h"
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

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
		if(cv_ec.code == cv::Error::StsAssert) { qDebug() << Qt::endl << TAG_NAME << "[ ASSREST ] " << cv_ec.msg; return;}
		if(cv_ec.code == cv::Error::BadROISize) { qDebug() << Qt::endl << TAG_NAME << "[ BAD ROI ] " << cv_ec.msg; return;}
          
		qDebug() << Qt::endl << TAG_NAME << cv_ec.what() << cv_ec.code;	
	}


}
