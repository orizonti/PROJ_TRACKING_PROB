#ifndef TEMPLATE_TRACKER_H
#define TEMPLATE_TRACKER_H

#include "./TRACKER_ROBUST/Tracker.h"
#include "image_processing_node.h"
#include "image_finder.h"
#include "thread_operation_nodes.h"
#include "register_settings.h"
#include <atomic>

class ImageTrackerTemplate : public ModuleImageProcessing
{
    Q_OBJECT
public:
    explicit ImageTrackerTemplate(QObject* parent = 0): ModuleImageProcessing("[TRACKER TEMPLATE]") 
    { 
      SetLowFrequencyProcessing();
      SizeROI = SettingsRegister::GetValue("PROCESSING_ROI1");
      QObject::connect(this,&ModuleImageProcessing::signalCoord, this, &ImageTrackerTemplate::SlotSetInput, Qt::QueuedConnection);
    };

    explicit ImageTrackerTemplate(int width, int height, int size ,QObject* parent = 0): 
             ModuleImageProcessing(width, height, size, "[TRACKER TEMPLATE]") 
    { 
      SetLowFrequencyProcessing();
    };
    ~ImageTrackerTemplate() {};

   MeasurePeriodNode FrameMeasureInput;
   MeasurePeriodNode FrameMeasureProcess;
   MeasurePeriodNode InputPortPeriod;
   int InputPeriod = 0;
        TrackerFirst NodeTracker;


   bool isTrackHold() override { return NodeTracker.isTrackHold(); }
   void TrackObject();
   void ProcessInput();

   void setInput(const QPair<float,float>& Coord) override; 

public  slots:
   void SlotSetInput(const QPair<float,float>& Coord) override;
   void SlotProcessImage(const cv::Mat& Image) override;
   void SlotProcessImage()                     override;
   void SlotResetProcessing()                  override;
   void SlotSelectObject(std::pair<float,float> PointRelative) override;
  
};
#endif 
