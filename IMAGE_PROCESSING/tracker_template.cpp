#include "tracker_template.h"
#include <qnamespace.h>
#include <QThread>




void ImageTrackerTemplate::SlotProcessImage()
{
  
  //==================================================================================
                           if(isIdle() || isDisabled()) return;
                               if(SourceImage->empty()) return; 
                                                FrameMeasureProcess++; FrameMeasureInput++;

                               if(SourceImage->getAvailableFrames() > 2) skipFrames();
                    *ImageInput = SourceImage->getImageToProcess().clone(); if((*ImageInput).empty()) return;  

                                          std::lock_guard<std::mutex> locker1(MutexImageAccess);
                                          std::lock_guard<std::mutex> locker2(MutexInput); 
  //==================================================================================
  ImageProcessing = *ImageInput; 
  ProcessInput(); 
                     ImageInput++; 
                  if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin(); 

  if( NodeTracker.isTrackHold()) PassCoordClass<float>::passCoord(); 
  //==================================================================================
                                                 FrameMeasureProcess++;
  
  MutexImageAccessDisplay.lock(); *ImageOutput = ImageProcessing; MutexImageAccessDisplay.unlock();
}

void ImageTrackerTemplate::SlotProcessImage(const cv::Mat& Image) 
{

                          if(Image.empty()) return;  FrameMeasureProcess++; FrameMeasureInput++;
                                          std::lock_guard<std::mutex> locker(MutexImageAccess);

                    *ImageInput = Image.clone(); 
  ImageProcessing = *ImageInput; 
  
	try                                { ProcessInput();; }
	catch (const cv::Exception& cv_ec) { std::cout << TAG_NAME.c_str() << cv_ec.what() << cv_ec.code;	}
	catch (const std::exception& e)    { std::cout << TAG_NAME.c_str() << "[CAUGTH EXCEPTION]" << e.what();	}

                     ImageInput++; 
                  if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin(); 

  if( NodeTracker.isTrackHold()) PassCoordClass<float>::passCoord(); 
                                                     FrameMeasureProcess++;

  MutexImageAccessDisplay.lock(); *ImageOutput = ImageProcessing; MutexImageAccessDisplay.unlock();
}

void ImageTrackerTemplate::ProcessInput()
{
  if(StateProcessing == StatesModule::Disabled) return;
  TrackObject(); 

  CoordsObject[0] = NodeTracker.GetObjectPos();
   RectsObject[0] = NodeTracker.GetObjectRect(); 
}

void ImageTrackerTemplate::TrackObject() { NodeTracker.trackObject(ImageProcessing); }


void ImageTrackerTemplate::SlotSelectObject(std::pair<float,float> PointRelative)
{
  std::lock_guard<std::mutex> locker(MutexInput); 
  CoordsObject[0] = std::make_pair(ImageProcessing.cols*PointRelative.first, ImageProcessing.rows*PointRelative.second);

  RectsObject[0] = cv::Rect(CoordsObject[0].first  - SizeROI/2, 
                            CoordsObject[0].second - SizeROI/2, SizeROI, SizeROI);

  NodeTracker.resetRectTrack(ImageProcessing,RectsObject[0]);
  StateProcessing = StatesModule::WorkTrack;     
  qDebug() << TAG_NAME << "[ SELECT AIMING OBJECT POINT ]" << CoordsObject[0].first << CoordsObject[0].second;
}


void ImageTrackerTemplate::setInput(const QPair<float,float>& Coord) 
{
  if(StateProcessing == StatesModule::Disabled) return;

  if(ImageInput->empty())
  {
   if(SourceImage == nullptr) return; 
     *ImageInput = SourceImage->getImageToProcess().clone(); 
   if(ImageInput->empty()) return;   
  }


  std::lock_guard<std::mutex> locker(MutexInput); 
  if(isTrackHold()) return;
  RectsObject[0] = cv::Rect(Coord.first  - SizeROI/2, 
                            Coord.second - SizeROI/2 , SizeROI, SizeROI);

                       CheckCorrectROI(RectsObject[0]);
  NodeTracker.setRectTrack(*ImageInput,RectsObject[0]);

  if(StateProcessing == StatesModule::Idle) SetStateActive(); 

  qDebug() << TAG_NAME << "[ INPUT COORD ]" << Coord.first << Coord.second << "[ ACTIVATE TRACK ]" << SizeROI;
};

void ImageTrackerTemplate::SlotResetProcessing() 
{ 
  qDebug() << Qt::endl; 
  StateProcessing = StatesModule::Idle; 

  MutexInput.lock(); NodeTracker.setStateIdle(); MutexInput.unlock();

  ModuleImageProcessing::SlotResetProcessing(); 
}

//qDebug() << OutputFilter::Filter(10) << "[ TEMPLATE INPUT ]" << ImageOutput.empty();

  //if( isLinkedSlave()) PassCoordClass::passCoord();
  //qDebug() << OutputFilter::Filter(10) << CoordsObject[0].first 
  //                                     << CoordsObject[0].second << FrameMeasureProcess.printPeriod();
  //

