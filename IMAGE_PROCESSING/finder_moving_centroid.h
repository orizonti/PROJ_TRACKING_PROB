#ifndef FINDER_CENTROID_H
#define FINDER_CENTROID_H

//#include <chrono>
#include <QObject>
#include <QDebug>

#include <QTimer>
#include <QImage>
#include <QString>
#include <opencv2/opencv.hpp>
#include <QFile> 

#include "optimization_threshold.h"
#include "image_processing_node.h"
#include "image_processing_node.h"
#include "filter_blotch.h"
#include "engine_statistics_track.h"
#include "tracker_centroid.h"


class FinderObjectMoving : public ModuleImageProcessing
{
    Q_OBJECT
public:

    FinderObjectMoving(QObject* parent = 0);
    FinderObjectMoving(int width, int height, int size ,QObject* parent); 
   ~FinderObjectMoving();

         FilterBlotchClass FilterBlotch;
    ContoursProcessorClass FindContours;
    ThresholdOptimizatorEngine ThresholdAdjuster{PROCESS_METHOD::PARALLEL_BY_DISPERSION};

    cv::Ptr<cv::BackgroundSubtractor> backSubstractor;

                                    bool isMoving = false;
    bool isTrackHold() override { return isMoving; }
    bool isLinksHoldTrack();
    bool CheckCentroid(); 

    void ProcessImage(cv::Mat& Image);

    using TrackerIterator = std::vector<std::shared_ptr<ImageTrackerCentroid>>::iterator ;


    std::vector<std::shared_ptr<ImageTrackerCentroid>      > Trackers;
    std::vector<std::shared_ptr<ImageTrackerCentroid>      > TrackersIdle;

    std::vector<std::shared_ptr<EstimatorTrackHold<float>> > TrackEstimators;

    std::vector<NodeCoordPassThinning<float>> NodesThinning{10};
                           std::vector<float> TrackEstimations{10};

    bool isTrackerHasDublicate(std::shared_ptr<ImageTrackerCentroid> TrackerCheck);
    bool isRectOnTrack(cv::Rect rect);
    void resetTracker(int number);

    NodeRectToCoord RectToCoord;

    //=====================================================
    std::function<void (cv::Mat&, cv::Mat&)> FilterSharpen;
    std::function<void (cv::Mat&, cv::Mat&)> FilterSobel;
    std::function<void (cv::Mat&, cv::Mat&)> FilterErosion;
              std::function<void (cv::Mat&)> NodeMedianFilter;
              std::function<void (cv::Mat&)> NodeThresholdFilter;
              std::function<void (cv::Mat&)> NodeErosion;
    void makeFilters();
    //=====================================================

   void resetState() override;
public  slots:
   void SlotResetProcessing() override;
   void SlotStartProcessing() override;
   void SlotStopProcessing()  override;
private  slots:
   void SlotProcessImage()    override;
};

#endif 
