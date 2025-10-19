#ifndef TEMPLATE_TRACKER_H
#define TEMPLATE_TRACKER_H

#include "./TRACKER_ROBUST/Tracker.h"
#include "image_processing_node.h"
#include "image_finder.h"
#include "thread_operation_nodes.h"

class ImageTrackerTemplate : public ModuleImageProcessing
{
    Q_OBJECT
public:
    explicit ImageTrackerTemplate(QObject* parent = 0) {};
    ~ImageTrackerTemplate() {};
    QString info = "[ TEMPLATE CPU ]";
    
    ImageTemplateFinder ImageFinder;

    FramePeriodMeasure FrameMeasureInput;
    FramePeriodMeasure FrameMeasureProcess;

    TrackerFirst NodeTracker;
    TrackHoldDetectorNode TrackHoldDetector{0.1,10};


   void FindObject();
   void TrackObject();
   void ProcessInput();
public  slots:
   void SlotProcessImage(const cv::Mat& Image) override;
   void SlotProcessImage() override;
   void SlotResetProcessing() override;
  
};
#endif 
