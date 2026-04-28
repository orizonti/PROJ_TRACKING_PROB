#include "tracker_template.h"




void ImageTrackerTemplate::SlotProcessImage()
{
  
                 if(SourceImage->empty()) return; FrameMeasureProcess++; FrameMeasureInput++;
                                                  MutexImageAccess.lock();

                    *ImageInput = SourceImage->getImageToProcess().clone(); 
                               if(SourceImage->getAvailableFrames() > 2) skipFrames();
                if((*ImageInput).empty()) { qDebug()<<OutputFilter::Filter(10) << "[ TEMPLATE EMPTY INPUT ]" ; return; } 

      ImageOutput = *ImageInput; 
  ImageProcessing = *ImageInput; ProcessInput(); 
                     ImageInput++; 
                  if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin(); 

                                                 FrameMeasureProcess++; MutexImageAccess.unlock();

  if( NodeTracker.isTrackHold()) emit ModuleImageProcessing::signalCoord(CoordsObject[0]); 
  
  if( isLinkedSlave()) PassCoordClass::passCoord();

  //qDebug() << OutputFilter::Filter(10) << CoordsObject[0].first 
  //                                     << CoordsObject[0].second << FrameMeasureProcess.printPeriod();
}

void ImageTrackerTemplate::SlotProcessImage(const cv::Mat& Image) 
{

                          if(Image.empty()) return;  FrameMeasureProcess++; FrameMeasureInput++;
                                                     MutexImageAccess.lock();
                    *ImageInput = Image.clone(); 

      ImageOutput = *ImageInput; 
  ImageProcessing = *ImageInput; ProcessInput();
                     ImageInput++; 
                  if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin(); 

  PassCoordClass<float>::passCoord(); 
                                                     MutexImageAccess.unlock();
                                                     FrameMeasureProcess++;
}

void ImageTrackerTemplate::ProcessInput()
{
  if(StateProcessing == StatesModule::Idle) return;
  TrackObject(); 

  CoordsObject[0] = NodeTracker.GetObjectPos();
   RectsObject[0] = NodeTracker.GetObjectRect(); 
}

void ImageTrackerTemplate::TrackObject() { NodeTracker.trackObject(ImageProcessing); }

void ImageTrackerTemplate::SlotSelectObject(std::pair<float,float> PointRelative)
{
  CoordsObject[0] = std::make_pair(ImageProcessing.cols*PointRelative.first, ImageProcessing.rows*PointRelative.second);

  RectsObject[0] = cv::Rect(CoordsObject[0].first  - SizeROI/2, 
                            CoordsObject[0].second - SizeROI/2, SizeROI, SizeROI);

  NodeTracker.setRectTrack(ImageProcessing,RectsObject[0]);

  qDebug() << TAG_NAME << "[ SELECT AIMING OBJECT POINT ]" << CoordsObject[0].first << CoordsObject[0].second;
}

void ImageTrackerTemplate::SlotResetProcessing() { }


void ImageTrackerTemplate::setInput(const QPair<float,float>& Coord) 
{
  if(StateProcessing == StatesModule::WorkTrack && ModeProcessing == ModesModule::Master) return;
  qDebug() << TAG_NAME << "[ INPUT COORD ]" << Coord.first << Coord.second << "[ ACTIVATE TRACK ]" << SizeROI;

  CoordsObject[0] = Coord; 
  RectsObject[0] = cv::Rect(CoordsObject[0].first  - SizeROI/2, 
                            CoordsObject[0].second - SizeROI/2 , SizeROI, SizeROI);

             if(SourceImage == nullptr) { qDebug() << TAG_NAME << "[ INIT NOT SOURCE ]"; };
  *ImageInput = SourceImage->getImageToProcess().clone(); 

  NodeTracker.setRectTrack(*ImageInput,RectsObject[0]);
  NodeTracker.trackObject(*ImageInput);
  SetStateActive();
};



//qDebug() << OutputFilter::Filter(10) << "[ TEMPLATE INPUT ]" << ImageOutput.empty();