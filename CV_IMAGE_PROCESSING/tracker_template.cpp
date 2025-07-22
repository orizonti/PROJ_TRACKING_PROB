#include "tracker_template.h"


void ImageTrackerTemplate::SlotProcessImage(const cv::Mat& Image) 
{
   ImageInput = Image.clone(); ImageProcessing = ImageInput;
   FrameMeasureProcess++;

   FrameMeasureInput++;
   ProcessInput();
   FrameMeasureInput++;
}

void ImageTrackerTemplate::SlotProcessImage()
{
                             if( SourceImage == nullptr) return;
                             if(!SourceImage->isFrameAvailable()) return;
                             FrameMeasureInput++;
                             FrameMeasureProcess++;
  MutexImageAccess.lock();
                    ImageInput = SourceImage->GetImageToProcess().clone(); 
                              if(SourceImage->GetAvailableFrames() > 2) SkipFrames();
  MutexImageAccess.unlock();
  ImageProcessing = ImageInput;
  FrameMeasureProcess++;

  FrameMeasureInput++;
  ProcessInput();
  FrameMeasureInput++;

}

void ImageTrackerTemplate::FindObject()
{
  ImageFinder.SlotProcessImage(ImageProcessing);
  CoordsObject[0] = ImageFinder.GetOutput();
  CoordsObject[0] >> TrackHoldDetector;

  RectsObject[0] = cv::Rect(CoordsObject[0].first , 
                            CoordsObject[0].second, 40, 40);

  if(TrackHoldDetector.isTrackHold()) NodeTracker.init(ImageProcessing,RectsObject[0]);
}

void ImageTrackerTemplate::TrackObject()
{
  NodeTracker.follow(ImageProcessing);
}


void ImageTrackerTemplate::ProcessInput()
{
                                                   FrameMeasureProcess++;
  if(TrackHoldDetector.isTrackHold()) 
       TrackObject();
  else FindObject();
//     NodeTracker.thread(ImageProcessing);

  CoordsObject[0] = NodeTracker.GetObjectPos();
   RectsObject[0] = NodeTracker.GetObjectRect(); 
                                                  FrameMeasureProcess++;

  qDebug() << OutputFilter::Filter(10) << CoordsObject[0].first 
                                       << CoordsObject[0].second << FrameMeasureProcess.printPeriod() << info;
                                       //<< "IS TRACK HOLD" << NodeTracker.isTrackHold();

  emit ImageSourceInterface::SignalNewImage(); PassCoord(); 

}

void ImageTrackerTemplate::SlotResetProcessing()
{
   qDebug() << TAG_NAME << "[ NUMBER ]" << NumberModule << "[ TEMPLATE RESET]";
   ModuleImageProcessing::SlotResetProcessing(); 
   TrackHoldDetector.Reset();
}

