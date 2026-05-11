#include "Tracker.h"
#include <QDebug>
#include <opencv2/highgui.hpp>
#include "debug_output_filter.h"

bool TrackerFirst::IsROIValid(cv::Rect& ROI, const cv::Mat& Image)
{
const int& ImageWidth  = (Image).cols;
const int& ImageHeight = (Image).rows;

  if((ROI.x + ROI.width  + 2) > ImageWidth  || 
     (ROI.y + ROI.height + 2) > ImageHeight ||
                                 ROI.x <= 0 || 
                                 ROI.y <= 0) return false;
  return true ;
}

TrackerFirst::TrackerFirst()
{
                                params.desc_npca       = cv::TrackerKCF::GRAY;
                                params.desc_pca        = cv::TrackerKCF::GRAY;
                                params.compressed_size = 1;
  tracker = TypeTracker::create(params);
    
}

TrackerFirst::~TrackerFirst() { tracker.release(); }

void TrackerFirst::resetRectTrack(const cv::Mat& image, cv::Rect rectAim)
{

   if(!IsROIValid(rectAim,image)) { rectAim = rect_template; qDebug() << TAG_NAME << "[ TRACK RECT IS NOT VALID ]"; };

   if(State == StatesModule::WorkTrack  || State == StatesModule::WorkSearch) 
   {
     tracker.release();
     tracker = TypeTracker::create(params); 
   }

	                      rect_template = rectAim;
   tracker->init(image, rect_template);
   State = StatesModule::WorkTrack;

   FLAG_INIT_TRACK = true;
   qDebug () << "[ TRACKER KCF INIT DONE]"; 
}

void TrackerFirst::setRectTrack(const cv::Mat& image, cv::Rect rectAim)
{
   if(FLAG_INIT_TRACK) return;
   resetRectTrack(image,rectAim);
}

void TrackerFirst::trackObject(cv::Mat& image, cv::Rect rect)
{
                           if(image.empty()) return;
                                                rect_template = rect;
        isTrackSuccess = tracker->update(image, rect_template); 
        State = StatesModule::WorkTrack;
}

void TrackerFirst::trackObject(cv::Mat& image)
{
                           if(image.empty()) return;
                           if(State == StatesModule::Idle) return;
        

        isTrackSuccess = tracker->update(image, rect_template); 

    if(!isTrackSuccess    ) { State = StatesModule::WorkSearch; FLAG_INIT_TRACK = false;  }; 
    if(!isTrackRectValid()) { State = StatesModule::Idle;      tracker.release(); }; 
    
    printState();
        
}

void TrackerFirst::printState()
{
    if(State == StatesModule::WorkSearch) qDebug() << " [TEMPLATE TRACKER FAIL ] [ SEARCH STATE]" ;
    if(State == StatesModule::Idle)       qDebug() << OutputFilter::Filter(20) << " [TEMPLATE TRACKER FAIL ] [ IDLE STATE]";
}




