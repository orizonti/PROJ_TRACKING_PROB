

#include "glib.h"
#include "gmodule.h"
#include "image_processing_node.h"
#include "interface_pass_coord.h"
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <qnamespace.h>

#include <chrono>
#include <debug_output_filter.h>
#include <QThread>

#include "finder_moving_centroid.h"

#include "debug_output_filter.h"
#include <algorithm>
#include "state_block_enum.h"


FinderObjectMoving::FinderObjectMoving(QObject* parent) : ModuleImageProcessing("[FINDER MOVING   ]",parent)
{
  makeFilters();
  SetLowFrequencyProcessing();
  for(int n = 0; n < 5; n++) 
  { 
           Trackers.push_back(std::make_shared<ImageTrackerCentroid>()); 
    TrackEstimators.push_back(std::make_shared<EstimatorTrackHold<float>>());
  }

  TrackerActive = Trackers.begin();
    TrackerIdle = Trackers.begin();
     TrackerEnd = Trackers.begin()+3;

  std::fill(TrackEstimations.begin(), TrackEstimations.end(),0);
}

FinderObjectMoving::FinderObjectMoving(int width, int height, int size ,QObject* parent): 
                      ModuleImageProcessing(width, height, size, "[FINDER MOVING   ]") 
{ 
  makeFilters();
  SetLowFrequencyProcessing();

  for(int n = 0; n < 5; n++) 
  { 
           Trackers.push_back(std::make_shared<ImageTrackerCentroid>()); 
    TrackEstimators.push_back(std::make_shared<EstimatorTrackHold<float>>());
  }

  TrackerActive = Trackers.begin();
    TrackerIdle = Trackers.begin();
     TrackerEnd = Trackers.begin()+3;
};

FinderObjectMoving::~FinderObjectMoving() { qDebug() << TAG_NAME << "[ DELETE ]"; }


bool FinderObjectMoving::isLinksHoldTrack()
{
    bool isLinksOnTrack = true;
  for(auto& link: Links) { isLinksOnTrack &= link->isTrackHold(); };                                     
  return isLinksOnTrack;
}

void FinderObjectMoving::SlotProcessImage()
{

  if( isLinksHoldTrack() ) { StateProcessing = StatesModule::Idle; } 
  if( SourceImage->empty() ) return;
  if( StateProcessing == StatesModule::Idle) {timerProcessImage.stop(); return; }
                                                       MutexImageAccess.lock();
                                                       FrameMeasureInput++;
                                                       FrameMeasureProcess++; 

  *ImageInput = SourceImage->getImageToProcess().clone(); if((*ImageInput).empty()) return;  
             if(SourceImage->getAvailableFrames() > 2) 
                SourceImage->skipFrames();

  ProcessImage(*ImageInput); ImageOutput = ImageProcessing;
                ImageInput++; if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin();

                                                       FrameMeasureProcess++;
                                                       MutexImageAccess.unlock();

  qDebug() << OutputFilter::Filter(20)<< "[ FIND MOVING OBJECT ] PERIOD" << FrameMeasureProcess.printPeriod();


  if( !isTrackHold() ) return;
  emit ModuleImageProcessing::signalCoord(CoordsObject[0]); 

}
//cv::cvtColor(Image, this->ImageInput, cv::COLOR_BGR2GRAY);
//
void FinderObjectMoving::SlotProcessImage(const cv::Mat& Image) 
{
                                                 FrameMeasureInput++; 
                                                 FrameMeasureProcess++; MutexImageAccess.lock();

                          if(Image.empty()) return;  
               *ImageInput = Image.clone(); 
  ProcessImage(*ImageInput); ImageOutput = ImageProcessing;
                ImageInput++; if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin();

                                                 FrameMeasureProcess++; MutexImageAccess.unlock();

  //qDebug() << OutputFilter::Filter(50)<< "[ FIND MOVING OBJECT ] PERIOD" << FrameMeasureProcess.printPeriod();

  if( !isTrackHold() ) return;
  emit ModuleImageProcessing::signalCoord(CoordsObject[0]); 

  if( isLinksHoldTrack() ) { StateProcessing = StatesModule::Idle; } 
       //PassCoordClass<float>::passCoord(); 
}

bool FinderObjectMoving::isTrackerHasDublicate(std::shared_ptr<ImageTrackerCentroid> TrackerCheck)
{
  for(auto& Tracker: Trackers) if(Tracker->isIntersects(TrackerCheck)) return true;
  return false;
}

bool FinderObjectMoving::isRectOnTrack(cv::Rect rect)
{
  for(auto& Tracker: Trackers) if(Tracker->isIntersects(NodeRectToCoord::Convert(rect))) return true;
  return false;
}

void FinderObjectMoving::ProcessImage(cv::Mat& Image)
{
    if(StateProcessing == StatesModule::Idle) return;

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));

    backSubstractor->apply(Image, ImageTemp1);
                    FilterErosion(ImageTemp1, ImageTemp2); 
                 cv::morphologyEx(ImageTemp2, ImageProcessing, cv::MORPH_OPEN, kernel); // morphological operations to remove noise and fill holes
                                 FindContours(ImageProcessing);


        for(auto Rect: FindContours)
        {
                                if(TrackerIdle == TrackerEnd) return; 
                                if(isRectOnTrack(Rect)      ) continue;
          Rect >> RectToCoord >> **TrackerIdle; 
                   TrackerActive = TrackerIdle; //qDebug() << "[ INIT TRACKER ]" << TrackerEnd - TrackerActive << (*TrackerActive)->isActive();
                                   TrackerIdle++; 
        }

        ImageProcessing >> *Trackers[0] >> *TrackEstimators[0] >> TrackEstimations[0];
        ImageProcessing >> *Trackers[1] >> *TrackEstimators[1] >> TrackEstimations[1];
        ImageProcessing >> *Trackers[2] >> *TrackEstimators[2] >> TrackEstimations[2];

        if(Trackers[2]->isActive() && isTrackerHasDublicate(Trackers[2])) { Trackers[2]->SlotResetProcessing(); TrackerIdle--; 
                                                                            TrackEstimators[2]->reset(); 
                                                                            TrackEstimations[2] = 0; }

        if(Trackers[1]->isActive() && isTrackerHasDublicate(Trackers[1])) { Trackers[1]->SlotResetProcessing(); TrackerIdle--; 
                                                                            TrackEstimators[1]->reset(); 
                                                                            TrackEstimations[1] = 0; }

        auto max = std::max_element(TrackEstimations.begin(), TrackEstimations.begin()+3);
        auto num = std::distance   (TrackEstimations.begin(), max);

         CoordsObject[0] = Trackers[num]->CoordsObject[0];
          RectsObject[0] = Trackers[num]->RectsObject[0];

        IS_MOVING = TrackEstimators[0]->EstimatorVelocity.isMoving() ||
                    TrackEstimators[1]->EstimatorVelocity.isMoving() ||
                    TrackEstimators[2]->EstimatorVelocity.isMoving();

                                                                                 FindContours.PrintContoursRect(ImageProcessing);

    qDebug() << OutputFilter::Filter(4) << "[TRACK1 ]" << TrackEstimators[0]->EstimatorVelocity.isMoving() << (int)TrackEstimations[0] 
                                        << "[TRACK2 ]" << TrackEstimators[1]->EstimatorVelocity.isMoving() << (int)TrackEstimations[1] 
                                        << "[TRACK3 ]" << TrackEstimators[2]->EstimatorVelocity.isMoving() << (int)TrackEstimations[2] 
                                        << "[MOVING]" << IS_MOVING;
}


void FinderObjectMoving::SlotResetProcessing() { }


void FinderObjectMoving::makeFilters()
{
    backSubstractor = cv::createBackgroundSubtractorMOG2();
    //==================================================================
    cv::Mat kernel1 = (cv::Mat_<double>(3,3) << 0, 0, 0, 
                                                0, 1, 0, 
                                                0, 0, 0);

    FilterSharpen = [kernel1,this](cv::Mat& Image, cv::Mat& ImageOutput)
    { 
    filter2D(Image, ImageOutput, -1 , kernel1, cv::Point(-1, -1), 0, 4);
    };
    //==================================================================

    cv::Mat kernel2 = (cv::Mat_<double>(3,3) << -1, 0, 1, 
                                                -2, 0, -2, 
                                                -1, 0, 1);
    FilterSobel = [kernel2,this](cv::Mat& Image, cv::Mat& ImageOutput)
    { 
    filter2D(Image, ImageOutput, -1 , kernel2, cv::Point(-1, -1), 0, 4);
    };
    //==================================================================

    int erosion_size = 2;
    int dilation_size = 2;
    //int erosion_type = cv::MORPH_RECT;
    int erosion_type = cv::MORPH_CROSS;
    int dilation_type = cv::MORPH_RECT;

    cv::Mat element = getStructuringElement( erosion_type,
                        cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                        cv::Point( erosion_size, erosion_size ) );

    FilterErosion = [element,this](cv::Mat& Image, cv::Mat& ImageOutput)
    { 
    erode( Image, ImageOutput, element );
    };
    //==================================================================

            NodeErosion = [this](cv::Mat& Image) { FilterErosion(Image,Image); };
       NodeMedianFilter = [this](cv::Mat& Image) { cv::medianBlur(Image, Image,3);; };
    NodeThresholdFilter = [this](cv::Mat& Image) { cv::threshold (Image, Image,10,256,cv::THRESH_BINARY); };

    std::vector<std::function<void (cv::Mat&)>> NodesList;
    NodesList.push_back(NodeErosion);
    NodesList.push_back(NodeMedianFilter);
    NodesList.push_back(NodeMedianFilter);
}

                                      
  //FilterSharpen(ImageProc1, ImageProc2); 
  //cv::medianBlur(Image      ,ImageProc1 ,7);
  //cv::medianBlur(ImageProc1 ,ImageProc2 ,7);
  //cv::threshold(ImageProc2 ,ImageProc1,Threshold,256,cv::THRESH_BINARY);
