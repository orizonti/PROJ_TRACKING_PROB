#include "tracker_template.h"
#include <qnamespace.h>
#include <QThread>

void ImageTrackerTemplate::SlotProcessImage()
{
  //==================================================================================
   //if(isIdle()) return;
   if(SourceImage->empty()) return; 

                                                FrameMeasureProcess++; FrameMeasureInput++;

                               if(SourceImage->getAvailableFrames() > 2) skipFrames();
                    *ImageInput = SourceImage->getImageToProcess().clone(); if((*ImageInput).empty()) return;  

  if(isTrackMiss()) 
  { MutexImageAccessDisplay.lock(); *ImageOutput = *ImageInput; 
    MutexImageAccessDisplay.unlock(); 
    qDebug() << OutputFilter::Filter(20) << "[TRACK MISS]";
    return; }

                                          std::lock_guard<std::mutex> locker1(MutexImageAccess);
                                          std::lock_guard<std::mutex> locker2(MutexInput); 
  //==================================================================================
  ImageProcessing = *ImageInput; 
  //ProcessInput(); 

	try                                { ProcessInput(); }
	catch (const cv::Exception& cv_ec) { std::cout << TAG_NAME.c_str() << cv_ec.what() << cv_ec.code;	}
	catch (const std::exception& e)    { std::cout << TAG_NAME.c_str() << "[CAUGTH EXCEPTION]" << e.what();	}
                     ImageInput++; 
                  if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin(); 

  if( NodeTracker.isTrackHold()) PassCoordClass<float>::passCoord(); else StateProcessing=StatesModule::Idle;
  //==================================================================================
                                                 FrameMeasureProcess++;
  
  MutexImageAccessDisplay.lock(); *ImageOutput = ImageProcessing; MutexImageAccessDisplay.unlock();
}


void ImageTrackerTemplate::ProcessInput()
{
                    NodeTracker.trackObject(ImageProcessing); 
  CoordsObject[0] = NodeTracker.GetObjectPos();
   RectsObject[0] = NodeTracker.GetObjectRect(); 
}

void ImageTrackerTemplate::SlotSelectObject(std::pair<float,float> PointRelative)
{
  if(SourceImage == nullptr) return; 

                              auto Size = SourceImage->getSizeImage();
  CoordsObject[1] = std::make_pair(Size.first*PointRelative.first, Size.second*PointRelative.second);

  if(ImageInput->empty())
  {
     *ImageInput = SourceImage->getImageToProcess().clone(); 
   if(ImageInput->empty()) return;   
  }

  std::lock_guard<std::mutex> guard(MutexInput);

  RectsObject[0] = cv::Rect(CoordsObject[1].first  - SizeROI/2, 
                            CoordsObject[1].second - SizeROI/2 , SizeROI, SizeROI);

                       CheckCorrectROI(RectsObject[0]);
  NodeTracker.setRectTrack(*ImageInput,RectsObject[0]);

  StateProcessing = StatesModule::WorkTrack; SetBlockOutput(false);
  qDebug() << TAG_NAME << "[ SELECT OBJECT ]" << CoordsObject[0].first << CoordsObject[0].second;
}


void ImageTrackerTemplate::setInput(const QPair<float,float>& Coord) 
{
                                   if(isDisabled()) return;   
                                   if(isTrackHold() && isModuleMaster()) return;
  if(SourceImage == nullptr) return; 
     ImageTemp3 = SourceImage->getImageToProcess().clone(); 

  std::lock_guard<std::mutex> guard(MutexInput);
  CoordsObject[1] = Coord;                  

  RectsObject[0] = cv::Rect(Coord.first  - SizeROI/2, 
                            Coord.second - SizeROI/2 , SizeROI, SizeROI);

                      CheckCorrectROI(RectsObject[0]);
  NodeTracker.setRectTrack(ImageTemp3,RectsObject[0]);

  StateProcessing = StatesModule::WorkTrack; SetBlockOutput(false);

  qDebug() << TAG_NAME << "[ INPUT COORD ]" << Coord.first << Coord.second << "[ ACTIVATE TRACK ]" << SizeROI;
};

void ImageTrackerTemplate::resetState() 
{
  NodeTracker.reset();
  StateProcessing = StatesModule::Idle; SetBlockOutput(true);
}


