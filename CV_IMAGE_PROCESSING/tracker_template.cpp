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
                    ImageInput = SourceImage->getImageToProcess().clone(); 
                              if(SourceImage->getAvailableFrames() > 2) skipFrames();
  MutexImageAccess.unlock();

                 if(ImageInput.empty()) return;
  ImageProcessing = ImageInput; 

  try
  {
  ProcessInput();                                                            
  }
  catch(const cv::Exception& e) 
  {
    const char* err_msg = e.what();
    qDebug() << TAG_NAME << "[CV EXCETION]" << err_msg;
  }

  FrameMeasureProcess++;

  //SEND TRACK COORD TO SLAVE TRACKER
  if( isLinkedSlave() && NodeTracker.isTrackHold()) emit ModuleImageProcessing::SignalTrackCoord(CoordsObject[0]); 
  //if( isLinkedSlave() && NodeTracker.isTrackHold()) { qDebug() << OutputFilter::Filter(20) << TAG_NAME << "PASS TO SLAVE: "; }
  

  //PASS TRACK COORD TO AIMING MODULE OR ELSE PassCoordClass
  if( !isLinkedSlave()) PassCoordClass::passCoord();
  //if(!isLinked()) PassCoordClass::passCoord();

  //qDebug() << OutputFilter::Filter(10) << TAG_NAME << "INPUT: " << QString::number(ImageProcessing.cols) << QString::number(ImageProcessing.rows);
 // qDebug() << OutputFilter::Filter(30) << TAG_NAME << CoordsObject[0].first 
 //                                                  << CoordsObject[0].second << FrameMeasureProcess.printPeriod();
  //
  //if(FrameMeasureProcess.getMilliseconds() > 20) qDebug() << TAG_NAME << "[ PROCESS TIME TO HIGH ] "
  //                                                                    << FrameMeasureProcess.getMilliseconds();
                                                                      
}

void ImageTrackerTemplate::ProcessInput()
{
  //if(TrackHoldDetector.isTrackHold()) TrackObject(); else FindObject(); //     NodeTracker.thread(ImageProcessing);
  TrackObject(); 

  CoordsObject[0] = NodeTracker.GetObjectPos();

   RectsObject[0] = NodeTracker.GetObjectRect(); 

   //qDebug() << OutputFilter::Filter(10) << "TEMPLATE TRACKER LINKED: " << PassCoordClass::isLinked();
}

void ImageTrackerTemplate::TrackObject() { NodeTracker.follow(ImageProcessing); }

void ImageTrackerTemplate::SlotSetAimPoint(std::pair<float,float> PointRelative)
{
  CoordsObject[0] = std::make_pair(ImageProcessing.cols*PointRelative.first, ImageInput.rows*PointRelative.second);

  RectsObject[0] = cv::Rect(CoordsObject[0].first  - ROI_SIZE/2, 
                            CoordsObject[0].second - ROI_SIZE/2, ROI_SIZE, ROI_SIZE);

  NodeTracker.init(ImageProcessing,RectsObject[0]);

  qDebug() << TAG_NAME << "[ SELECT AIMING OBJECT POINT ]" << CoordsObject[0].first << CoordsObject[0].second;
}

void ImageTrackerTemplate::FindObject()
{
//  ImageFinder.SlotProcessImage(ImageProcessing);
//  CoordsObject[0] = ImageFinder.getOutput();
//  CoordsObject[0] >> TrackHoldDetector;
//
//  RectsObject[0] = cv::Rect(CoordsObject[0].first , 
//                            CoordsObject[0].second, 40, 40);
//
//  if(TrackHoldDetector.isTrackHold()) NodeTracker.init(ImageProcessing,RectsObject[0]);
}




void ImageTrackerTemplate::SlotResetProcessing()
{
   qDebug() << TAG_NAME << "[ NUMBER ]" << NumberModule << "[ TEMPLATE RESET]";
   ModuleImageProcessing::SlotResetProcessing(); 
   //TrackHoldDetector.Reset();
}

