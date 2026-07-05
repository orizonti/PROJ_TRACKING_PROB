

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
  SetBlockOutput(true);

  makeFilters();
  for(int n = 0; n < 5; n++) 
  { 
           Trackers.push_back(std::make_shared<ImageTrackerCentroid>()); 
           Trackers.back()->SetSlaveMode(ModuleImageProcessing::ModesModule::SlavePassive);
           Trackers.back()->SetStateIdle();
           Trackers.back()->TAG_NAME = QString("[TRACKER] %1").arg(n).toStdString();

    TrackEstimators.push_back(std::make_shared<EstimatorTrackHold<float>>());
  }
  TrackersIdle = Trackers; std::reverse(TrackersIdle.begin(), TrackersIdle.end());
  for(auto& Tracker: TrackersIdle) qDebug() << "[IDLE]" << Tracker->TAG_NAME.c_str() << Tracker->isIdle();


  std::fill(TrackEstimations.begin(), TrackEstimations.end(),0);
}

FinderObjectMoving::FinderObjectMoving(int width, int height, int size ,QObject* parent): 
                      ModuleImageProcessing(width, height, size, "[FINDER MOVING   ]") 
{ 
  SetBlockOutput(true);
  makeFilters();

  for(int n = 0; n < 5; n++) 
  { 
           Trackers.push_back(std::make_shared<ImageTrackerCentroid>()); 
           Trackers.back()->SetSlaveMode(ModuleImageProcessing::ModesModule::SlavePassive);
           Trackers.back()->SetStateIdle();

    TrackEstimators.push_back(std::make_shared<EstimatorTrackHold<float>>());
  }

  TrackersIdle = Trackers; std::reverse(TrackersIdle.begin(), TrackersIdle.end());
                           
  for(auto& Tracker: TrackersIdle) qDebug() << "[IDLE]" << Tracker->TAG_NAME.c_str() << Tracker->isIdle();
};

FinderObjectMoving::~FinderObjectMoving() { qDebug() << TAG_NAME << "[ DELETE ]"; }


bool FinderObjectMoving::isLinksHoldTrack()
{
                      bool isLinksOnTrack = false;
  for(auto& link: Links) { isLinksOnTrack |= link->isTrackHold(); };                                     
                    return isLinksOnTrack;
}


bool FinderObjectMoving::isTrackerHasDublicate(std::shared_ptr<ImageTrackerCentroid> TrackerCheck)
{
  for(auto& Tracker: Trackers)
  {
    if(Tracker->isIdle())  continue;
    if(Tracker == TrackerCheck) continue;
    if(Tracker->isIntersects(TrackerCheck))
    {
      qDebug() << "TRACKER"      << Tracker->TAG_NAME.c_str() 
               << "[INTERSECTS]" << TrackerCheck->TAG_NAME.c_str();
      return true;
    }
  }

  return false;
}


bool FinderObjectMoving::isRectOnTrack(cv::Rect rect)
{
  for(auto& Tracker: Trackers) 
  {
    if(Tracker->isIntersects(NodeRectToCoord::Convert(rect)) &&
       Tracker->isTrackHold()) return true;
  }
  return false;
}

void FinderObjectMoving::ProcessImage(cv::Mat& Image)
{
    if(StateProcessing == StatesModule::Disabled) return;

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));

    backSubstractor->apply(Image, ImageTemp1); //FilterErosion(ImageTemp1, ImageTemp2); 
                 cv::morphologyEx(ImageTemp1, ImageProcessing, cv::MORPH_OPEN, kernel); 
                                 FindContours(ImageProcessing);


        for(auto Rect: FindContours)
        {
                                if(TrackersIdle.empty()) break;; 
                                if(isRectOnTrack(Rect) ) continue;

          auto Tracker = TrackersIdle.back(); TrackersIdle.pop_back();
          Rect >> RectToCoord >> *Tracker; 

          qDebug() << Tracker->TAG_NAME << "[INIT TRACKER]" << Tracker->isTrackHold() << Tracker->isIdle();
        }

        ImageProcessing >> *Trackers[0] >> *TrackEstimators[0] >> TrackEstimations[0];
        ImageProcessing >> *Trackers[1] >> *TrackEstimators[1] >> TrackEstimations[1];
        ImageProcessing >> *Trackers[2] >> *TrackEstimators[2] >> TrackEstimations[2];

        if(Trackers[2]->isTrackHold() && isTrackerHasDublicate(Trackers[2])) resetTracker(2);
        if(Trackers[1]->isTrackHold() && isTrackerHasDublicate(Trackers[1])) resetTracker(1);

        auto max = std::max_element(TrackEstimations.begin(), TrackEstimations.begin()+3);
        auto num = std::distance   (TrackEstimations.begin(), max);

        if(*max > 80) 
        {
         CoordsObject[0] = Trackers[num]->CoordsObject[0];
          RectsObject[0] = Trackers[num]->RectsObject[0];
        }

           isMoving = TrackEstimators[0]->EstimatorVelocity.isMoving() ||
                      TrackEstimators[1]->EstimatorVelocity.isMoving() ||
                      TrackEstimators[2]->EstimatorVelocity.isMoving();
        if(isMoving) StateProcessing = StatesModule::WorkTrack;

         FindContours.PrintContoursRect(ImageProcessing);


    qDebug() << OutputFilter::Filter(20) 
    << Trackers[0]->TAG_NAME.c_str() << Trackers[0]->isIdle() << int(TrackEstimations[0])
    << Trackers[1]->TAG_NAME.c_str() << Trackers[1]->isIdle() << int(TrackEstimations[1])
    << Trackers[2]->TAG_NAME.c_str() << Trackers[2]->isIdle() << int(TrackEstimations[2])
    << Trackers[3]->TAG_NAME.c_str() << Trackers[3]->isIdle() << int(TrackEstimations[3]);
}

void FinderObjectMoving::resetTracker(int number)
{
   qDebug() << TAG_NAME.c_str() << "[RESET TRACKER]" << number;
   Trackers[number]->SlotResetProcessing(); 
   TrackEstimators[number]->reset(); 
   TrackEstimations[number] = 0; 
   TrackersIdle.push_back(Trackers[number]);
}

void FinderObjectMoving::SlotProcessImage()
{
  if( isLinksHoldTrack() ) 
  { 
    for(auto& link: Links) { qDebug() << link->TAG_NAME.c_str() << "[ HOLD ]" << link->isTrackHold(); };                                     
    qDebug() << Qt::endl << TAG_NAME.c_str() << "[LINKED HOLD]";
    SlotStopProcessing(); 
  } 
  std::lock_guard<std::mutex> locker(MutexImageAccess);

            if( SourceImage->empty() ) return;
                                                       FrameMeasureInput++;
                                                       FrameMeasureProcess++; 
  *ImageInput = SourceImage->getImageToProcess().clone(); if((*ImageInput).empty()) return;  
             if(SourceImage->getAvailableFrames() > 2) 
                SourceImage->skipFrames();

  ProcessImage(*ImageInput); 
                ImageInput++; if(ImageInput == ImagesInput.end()) ImageInput = ImagesInput.begin();

                                                       FrameMeasureProcess++;

  MutexImageAccessDisplay.lock(); *ImageOutput = ImageProcessing; MutexImageAccessDisplay.unlock();

  if(isTrackHold()) SetBlockOutput(false); PassCoordClass<float>::passCoord();
  //if(isTrackHold()) PassCoordClass<float>::passCoord();

}

void FinderObjectMoving::SlotStartProcessing() 
{ 
   qDebug() << TAG_NAME.c_str() << "[START PROCESSING]" << "[IDLE]"; 
  for(auto& Tracker: Links) Tracker->SetStateIdle();

    StateProcessing = StatesModule::Idle; SetBlockOutput(true);
  if(ModeProcessing == ModesModule::SlavePassive) return;

  SourceImage->skipFrames();
  
  if(!timerProcessImage.isActive()) timerProcessImage.start(); 

}

void FinderObjectMoving::SlotResetProcessing() 
{ 
  qDebug() << TAG_NAME.c_str() << "[RESET PROCESSING]";
  resetState();

  SlotStartProcessing();
}

void FinderObjectMoving::SlotStopProcessing() 
{
  qDebug() << TAG_NAME.c_str() << "[STOP PROCESSING]" << "[DISABLED]";
  resetState(); 

  if(timerProcessImage.isActive()) timerProcessImage.stop(); 
  StateProcessing = StatesModule::Disabled; 
}

void FinderObjectMoving::resetState()
{
  StateProcessing = StatesModule::Idle; SetBlockOutput(true);
  isMoving = false;
  FindContours.reset();

  for(int n = 0; n < TrackEstimations.size(); n++) TrackEstimations[n] = 0;

  for(auto& Estimator: TrackEstimators) Estimator->reset(); 
  for(auto& Tracker: Trackers) { Tracker->SetStateIdle(); }

  TrackersIdle = Trackers; std::reverse(TrackersIdle.begin(), TrackersIdle.end());

}

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
//   auto Diff = Trackers[1]->CoordsObject[0] - Trackers[0]->CoordsObject[0];
//   auto dist = std::hypot(Diff.first,Diff.second);
//
//   auto Diff2 = Trackers[2]->CoordsObject[0] - Trackers[0]->CoordsObject[0];
//   auto dist2 = std::hypot(Diff.first,Diff.second);
//    qDebug() << OutputFilter::Filter(10) 
//    << "[TRACK1 ]" << Trackers[0]->isTrackHold() << Trackers[0]->CoordsObject[0].first << Trackers[0]->CoordsObject[0].second
//    << "[TRACK2 ]" << Trackers[1]->isTrackHold() << Trackers[1]->CoordsObject[0].first << Trackers[1]->CoordsObject[0].second
//    << "[TRACK3 ]" << Trackers[2]->isTrackHold() << Trackers[2]->CoordsObject[0].first << Trackers[2]->CoordsObject[0].second
//    << "[MOVING]" << IS_MOVING << dist << dist2;
                                      
  //FilterSharpen(ImageProc1, ImageProc2); 
  //cv::medianBlur(Image      ,ImageProc1 ,7);
  //cv::medianBlur(ImageProc1 ,ImageProc2 ,7);
  //cv::threshold(ImageProc2 ,ImageProc1,Threshold,256,cv::THRESH_BINARY);
