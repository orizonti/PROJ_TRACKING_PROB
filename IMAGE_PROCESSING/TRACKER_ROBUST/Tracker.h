#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include <opencv2/core/ocl.hpp>
#include <QPair>
#include <atomic>
#include <QString>

using TypeTracker = cv::tracking::TrackerKCF;

class TrackerFirst
{
public:
	 TrackerFirst();
	~TrackerFirst();
	cv::Ptr<cv::TrackerKCF> tracker;
   QString TAG_NAME{"TRACKER KCF"};
   std::atomic<bool> isInitDone = false;

   enum class StatesModule { Disabled = 0, Active = 1, WorkTrack = 2};

   StatesModule State{StatesModule::Disabled};

	 void setRectTrack(const cv::Mat& image, cv::Rect rectAim);
	 void resetRectTrack(const cv::Mat& image, cv::Rect rectAim);

	 void trackObject(cv::Mat& image);
	 void trackObject(cv::Mat& image, cv::Rect rect);

   bool isDisabled()       { return State == StatesModule::Disabled; };
   bool isTrackHold()      { return State == StatesModule::WorkTrack; };
   bool isTrackRectValid() { return rect_template.width > 0 && rect_template.height > 0; }; 

   void setStateDisabled() { State = StatesModule::Disabled; } 
   void setStateActive  () { State = StatesModule::Active; } 
   void reset(); 

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

   bool IsROIValid(cv::Rect& ROI, const cv::Mat& Image);

   void printState();
private:
 	            cv::Rect rect_template{10,10,60,60};
   TypeTracker::Params params{ TypeTracker::Params() };

   void errorEmptyImage(bool print); 
   void errorRectInvalid(bool print);

};


