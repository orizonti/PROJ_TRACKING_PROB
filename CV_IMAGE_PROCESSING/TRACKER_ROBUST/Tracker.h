#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"
#include <opencv2/core/ocl.hpp>
#include <QPair>
#include <atomic>

class TrackerFirst
{
public:
	 TrackerFirst();
	~TrackerFirst();
	cv::Ptr<cv::TrackerKCF> tracker;

  cv::Mat TemplateImage;
  cv::Mat ProcessImage;
  cv::UMat ProcessImageGPU;

	void   init(const cv::Mat& image, cv::Rect rectAim);
	void   find(cv::Mat& image);
	void follow(cv::Mat& image);
	 int thread(cv::Mat& image);

   bool isTrackHold() { return follow_flag; };

   QPair<float,float> ObjectPos{0.0,0.0};
   QPair<float,float> ObjectRect{0.0,0.0};

   QPair<float,float> GetObjectPos() 
   { 
     ObjectPos.first = bbox.x + bbox.width/2;
    ObjectPos.second = bbox.y + bbox.height/2;
     return ObjectPos; 
   };

   cv::Rect& GetObjectRect() { return bbox; };

private:
	void update(cv::Mat& image, cv::Rect& bbox);

	void predict();

	cv::KalmanFilter KF = cv::KalmanFilter(4, 2, 0);

  std::atomic<bool> init_follow = false;
  std::atomic<bool> follow_flag = false;

	cv::Rect bbox;
	cv::Rect bbox_prev;

	int iterator = 0;
	int iterator2 = 0;

	cv::Point predicted_coords;

};


//class passCoords
//{
//public:
//	std::vector<int> coords = { 0, 0 };
//	void avg(cv::Rect bbox1, cv::Rect bbox2);
//};
