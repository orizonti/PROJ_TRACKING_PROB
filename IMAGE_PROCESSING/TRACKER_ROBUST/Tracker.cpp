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

   //qDebug() << TAG_NAME.toStdString().c_str() << "[RESET INIT]" << "[IMAGE]" << image.cols << image.rows 
   //                                                             << "[RECT ]" << rectAim.x << rectAim.y << rectAim.width << rectAim.height;
   qDebug() << TAG_NAME.toStdString().c_str() << "[SET RECT TRACK]";
                if(image.empty()) {  errorEmptyImage(false); return;}
   if(!IsROIValid(rectAim,image)) { errorRectInvalid(false); return;}
   reset();

   rect_template = rectAim;

   tracker->init(image, rect_template);
   State = StatesModule::WorkTrack;              
}

void TrackerFirst::reset() 
{
   qDebug() << TAG_NAME.toStdString().c_str() << "[RESET STATE]";
   if(State == StatesModule::WorkTrack)
   {
   tracker.release();
   tracker = TypeTracker::create(params);        
   }
   State = StatesModule::Disabled;
}

void TrackerFirst::setRectTrack(const cv::Mat& image, cv::Rect rectAim)
{
	try                                { resetRectTrack(image,rectAim); }
	catch (const cv::Exception& cv_ec) { std::cout << TAG_NAME.toStdString() << cv_ec.what() << cv_ec.code;	}
	catch (const std::exception& e)    { std::cout << TAG_NAME.toStdString() << "[CAUGTH EXCEPTION]" << e.what();	}
}

void TrackerFirst::errorEmptyImage(bool print)  { if(print) qDebug() << TAG_NAME.toStdString().c_str() << "[EMPTY IMAGE]"; }
void TrackerFirst::errorRectInvalid(bool print) { if(print) qDebug() << TAG_NAME.toStdString().c_str() << "[RECT INVALID]"; }


void TrackerFirst::trackObject(cv::Mat& image, cv::Rect rect)
{
                           if(image.empty()) return;
                           if(State == StatesModule::Disabled) return;
                                                rect_template = rect;
        isTrackSuccess = tracker->update(image, rect_template); 

        if(!isTrackSuccess    ) { State = StatesModule::Active; }; 
        if(!isTrackRectValid()) { State = StatesModule::Disabled;       }; printState();
}

void TrackerFirst::trackObject(cv::Mat& image)
{
                           if(image.empty()) return;
                           if(State == StatesModule::Disabled) return;
        
        isTrackSuccess = tracker->update(image, rect_template); 

    if(!isTrackSuccess    ) { State = StatesModule::Active; }; 
    if(!isTrackRectValid()) { State = StatesModule::Disabled;      tracker.release(); }; 
    
    printState();
        
}

void TrackerFirst::printState()
{
    if(State == StatesModule::Disabled) qDebug() << " [TEMPLATE TRACKER FAIL ] [ DISABLEDJ ]" ;
    if(State == StatesModule::Active)   qDebug() << " [TEMPLATE TRACKER FAIL ] [ ACTIVE    ]";
    //if(State == StatesModule::WorkTrack)  qDebug() << OutputFilter::Filter(50) << " [TEMPLATE TRACKER ] [ WORK STATE]";
}




