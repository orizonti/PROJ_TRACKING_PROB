#include "tracker_template.h"




void ImageTrackerTemplate::SlotProcessImage()
{
  
	try 
	{ 
                                                FrameMeasureProcess++; FrameMeasureInput++;
                               if(SourceImage->empty()) return; 
                               if(SourceImage->getAvailableFrames() > 2) skipFrames();
                    *ImageInput = SourceImage->getImageToProcess().clone(); if((*ImageInput).empty()) return;  

                                                                         MutexImageAccess.lock();
  //==================================================================================
      ImageOutput = *ImageInput; 
  ImageProcessing = *ImageInput; ProcessInput(); 
                     ImageInput++; 
                  if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin(); 
  //if( NodeTracker.isTrackHold()) emit ModuleImageProcessing::signalCoord(CoordsObject[0]); 
  if( NodeTracker.isTrackHold()) PassCoordClass<float>::passCoord(); 
  //==================================================================================

                                                 FrameMeasureProcess++; MutexImageAccess.unlock();
	}
	catch (const cv::Exception& cv_ec) 
	{ 
		if(cv_ec.code == cv::Error::StsAssert)  
    { std::cout << TAG_NAME.toStdString() << "[ ASSERTION FAILED ] " << cv_ec.msg << std::endl; return;}
		if(cv_ec.code == cv::Error::BadROISize) 
    { std::cout << TAG_NAME.toStdString() << "[ BAD ROI ] " << cv_ec.msg << std::endl; return;}
                                              std::cout << TAG_NAME.toStdString() << cv_ec.what() << cv_ec.code;	
	}



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

  NodeTracker.resetRectTrack(ImageProcessing,RectsObject[0]);

  qDebug() << TAG_NAME << "[ SELECT AIMING OBJECT POINT ]" << CoordsObject[0].first << CoordsObject[0].second;
}

void ImageTrackerTemplate::SlotResetProcessing() { }


void ImageTrackerTemplate::setInput(const QPair<float,float>& Coord) 
{
  if( isTrackHold() && ModeProcessing == ModesModule::Master) return;

  qDebug() << TAG_NAME << "[ INPUT COORD ]" << Coord.first << Coord.second << "[ ACTIVATE TRACK ]" << SizeROI;

                if(SourceImage == nullptr)  return; 
     *ImageInput = SourceImage->getImageToProcess().clone(); 
   if(ImageInput->empty()) return;   

                                                          MutexInput.lock();
   RectsObject[0] = cv::Rect(Coord.first  - SizeROI/2, 
                             Coord.second - SizeROI/2 , SizeROI, SizeROI);

                       CheckCorrectROI(RectsObject[0]);
  NodeTracker.setRectTrack(*ImageInput,RectsObject[0]);
  StateProcessing = StatesModule::WorkTrack;  
                                                          MutexInput.unlock();


};



//qDebug() << OutputFilter::Filter(10) << "[ TEMPLATE INPUT ]" << ImageOutput.empty();

  //if( isLinkedSlave()) PassCoordClass::passCoord();
  //qDebug() << OutputFilter::Filter(10) << CoordsObject[0].first 
  //                                     << CoordsObject[0].second << FrameMeasureProcess.printPeriod();
