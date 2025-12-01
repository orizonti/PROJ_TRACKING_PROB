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

  MutexImageAccess.lock();
                    ImageInput = SourceImage->getImageToProcess().clone(); 
                              if(SourceImage->getAvailableFrames() > 2) skipFrames();
  MutexImageAccess.unlock();

  ImageProcessing = ImageInput;
  //qDebug() << OutputFilter::Filter(10) << TAG_NAME << "INPUT: " << QString::number(ImageProcessing.cols) << QString::number(ImageProcessing.rows);

  FrameMeasureProcess++; ProcessInput(); FrameMeasureProcess++;

  //qDebug() << OutputFilter::Filter(10) << CoordsObject[0].first 
  //                                     << CoordsObject[0].second << FrameMeasureProcess.printPeriod() << info;
                                       //<< "IS TRACK HOLD" << NodeTracker.isTrackHold();
}

void ImageTrackerTemplate::ProcessInput()
{
  //if(TrackHoldDetector.isTrackHold()) TrackObject(); else FindObject(); //     NodeTracker.thread(ImageProcessing);
  TrackObject(); 

  CoordsObject[0] = NodeTracker.GetObjectPos();
   RectsObject[0] = NodeTracker.GetObjectRect(); 

   PassCoordClass::passCoord(); 
   //qDebug() << OutputFilter::Filter(10) << "TEMPLATE TRACKER LINKED: " << PassCoordClass::isLinked();
}

void ImageTrackerTemplate::TrackObject() { NodeTracker.follow(ImageProcessing); }

void ImageTrackerTemplate::SlotSetAimPoint(std::pair<float,float> PointRelative)
{
  CoordsObject[0] = std::make_pair(ImageProcessing.cols*PointRelative.first, ImageInput.rows*PointRelative.second);

  int roi_size = 80;
  RectsObject[0] = cv::Rect(CoordsObject[0].first  - roi_size/2, 
                            CoordsObject[0].second - roi_size/2, roi_size, roi_size);

  NodeTracker.init(ImageProcessing,RectsObject[0]);

  qDebug() << TAG_NAME << "[ SELECT AIMING OBJECT POINT ]" << CoordsObject[0].first << CoordsObject[0].second;
}

void ImageTrackerTemplate::FindObject()
{
  ImageFinder.SlotProcessImage(ImageProcessing);
  CoordsObject[0] = ImageFinder.getOutput();
  CoordsObject[0] >> TrackHoldDetector;

  RectsObject[0] = cv::Rect(CoordsObject[0].first , 
                            CoordsObject[0].second, 40, 40);

  if(TrackHoldDetector.isTrackHold()) NodeTracker.init(ImageProcessing,RectsObject[0]);
}




void ImageTrackerTemplate::SlotResetProcessing()
{
   qDebug() << TAG_NAME << "[ NUMBER ]" << NumberModule << "[ TEMPLATE RESET]";
   ModuleImageProcessing::SlotResetProcessing(); 
   TrackHoldDetector.Reset();
}

