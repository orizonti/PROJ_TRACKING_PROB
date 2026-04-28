#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include <opencv2/core/ocl.hpp>
#include <QPair>
#include <atomic>

using TypeTracker = cv::tracking::TrackerKCF;

class TrackerFirst
{
public:
	 TrackerFirst();
	~TrackerFirst();
	cv::Ptr<cv::TrackerKCF> tracker;

   enum class StatesModule { Idle = 0, WorkSearch = 1, WorkTrack = 2};

   StatesModule State{StatesModule::Idle};

	 void setRectTrack(const cv::Mat& image, cv::Rect rectAim);
	 void trackObject(cv::Mat& image);

   bool isTrackHold() { return State == StatesModule::WorkTrack; };
   bool isTrackRectValid() { return rect_template.width > 0 && rect_template.height > 0; }; 

                 bool isTrackSuccess = false;
   QPair<float,float> ObjectPos {0.0,0.0};
   QPair<float,float> ObjectRect{0.0,0.0};

   QPair<float,float> GetObjectPos() 
   { 
    ObjectPos.first = rect_template.x + rect_template.width/2;
    ObjectPos.second = rect_template.y + rect_template.height/2;
     return ObjectPos; 
   };

   cv::Rect& GetObjectRect() { return rect_template; };

   void printState();
private:
 	            cv::Rect rect_template{10,10,60,60};
   TypeTracker::Params params{ TypeTracker::Params() };

};


