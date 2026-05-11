#include "Tracker.h"
#include <QDebug>
#include <opencv2/highgui.hpp>
#include "debug_output_filter.h"

bool TrackerFirst::IsROIValid(cv::Rect& ROI, const cv::Mat& Image)
{

  if((ROI.x + ROI.width  + 2) > Image.cols  || 
     (ROI.y + ROI.height + 2) > Image.rows  ||
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

   if(image.empty()) { qDebug() << "[TRACKER KCF] RESET INPUT EMPTY"; } 
   if(!IsROIValid(rectAim,image)) { rectAim = rect_template; qDebug() << TAG_NAME << "[ TRACK RECT IS NOT VALID ]"; };

                                                 qDebug() << "[TRACKER KCF] RELEASE RESET";
   tracker.release();
   tracker = TypeTracker::create(params);        qDebug() << "[TRACKER KCF] INIT AFTER RESET";
	                      rect_template = rectAim;
   tracker->init(image, rect_template);
   State = StatesModule::WorkTrack;              qDebug () << "[TRACKER KCF] INIT DONE"; 
}

void TrackerFirst::setRectTrack(const cv::Mat& image, cv::Rect rectAim)
{
	try                                { resetRectTrack(image,rectAim); }
	catch (const cv::Exception& cv_ec) { std::cout << TAG_NAME.toStdString() << cv_ec.what() << cv_ec.code;	}
	catch (const std::exception& e)    { std::cout << TAG_NAME.toStdString() << "[CAUGTH EXCEPTION]" << e.what();	}
}

void TrackerFirst::trackObject(cv::Mat& image, cv::Rect rect)
{
                           if(image.empty()) return;
                                                rect_template = rect;
        isTrackSuccess = tracker->update(image, rect_template); 

                                  State = StatesModule::WorkTrack;
        if(!isTrackSuccess    ) { State = StatesModule::WorkSearch; }; 
        if(!isTrackRectValid()) { State = StatesModule::Idle;       }; printState();
}

void TrackerFirst::trackObject(cv::Mat& image)
{
                           if(image.empty()) return;
                           if(State == StatesModule::Idle) return;
        
        isTrackSuccess = tracker->update(image, rect_template); 

    if(!isTrackSuccess    ) { State = StatesModule::WorkSearch; }; 
    if(!isTrackRectValid()) { State = StatesModule::Idle;      tracker.release(); }; 
    
    printState();
        
}

void TrackerFirst::printState()
{
    if(State == StatesModule::WorkSearch) qDebug() << " [TEMPLATE TRACKER FAIL ] [ SEARCH STATE]" ;
    if(State == StatesModule::Idle)       qDebug() << " [TEMPLATE TRACKER FAIL ] [ IDLE STATE]";
    //if(State == StatesModule::WorkTrack)  qDebug() << OutputFilter::Filter(50) << " [TEMPLATE TRACKER ] [ WORK STATE]";
}




