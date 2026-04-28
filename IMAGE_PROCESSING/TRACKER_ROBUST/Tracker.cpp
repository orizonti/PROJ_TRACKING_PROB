#include "Tracker.h"
#include <QDebug>
#include <opencv2/highgui.hpp>
#include "debug_output_filter.h"

TrackerFirst::TrackerFirst()
{
  qDebug() << "[ CREATE KCF TRACKER ]";
                                params.desc_npca       = cv::TrackerKCF::GRAY;
                                params.desc_pca        = cv::TrackerKCF::GRAY;
                                params.compressed_size = 1;
  tracker = TypeTracker::create(params);
    
}

TrackerFirst::~TrackerFirst() { tracker.release(); }

void TrackerFirst::setRectTrack(const cv::Mat& image, cv::Rect rectAim)
{

   qDebug() << "[ INIT TRACKER ]"  << rectAim.x << rectAim.y 
                                   << rectAim.width << rectAim.height;

   if(State == StatesModule::WorkTrack  || State == StatesModule::WorkSearch) 
   {
     tracker.release();
     tracker = TypeTracker::create(params); qDebug () << "[ TRACKER RESET ]"; 
   }

	                      rect_template = rectAim;
   tracker->init(image, rect_template);
   State = StatesModule::WorkTrack;
}

void TrackerFirst::trackObject(cv::Mat& image)
{
                           if(State == StatesModule::Idle) return;

        isTrackSuccess = tracker->update(image, rect_template); 

    if(!isTrackSuccess    ) { State = StatesModule::WorkSearch;                   }; 
    if(!isTrackRectValid()) { State = StatesModule::Idle;      tracker.release(); }; printState();
        
}

void TrackerFirst::printState()
{
    if(!isTrackSuccess    ) qDebug() << OutputFilter::Filter(20) << " [TEMPLATE TRACKER FAIL ] [ SEARCH STATE]" ;
    if(!isTrackRectValid()) qDebug() << OutputFilter::Filter(20) << " [TEMPLATE TRACKER FAIL ] [ IDLE STATE]";
}




