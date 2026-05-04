#ifndef TRACKER_CENTROID_H
#define TRACKER_CENTROID_H

//#include <chrono>
#include <QObject>
#include <QDebug>

#include <QTimer>
#include <QImage>
#include <QString>
#include <memory>
#include <opencv2/opencv.hpp>
#include <QFile> 
#include <chrono>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <tuple>
#include <mutex>


#include "interface_pass_coord.h"
#include "interface_image_source.h"
#include "engine_statistics.h"

#include "thread_operation_nodes.h"
#include "utilite.h"

#include "optimization_threshold.h"
#include "./TRACKER_ROBUST/Tracker.h"
#include "image_processing_node.h"
#include "contour_processing.h"
#include "image_processing_node.h"
#include "filter_blotch.h"


class ImageTrackerCentroid : public ModuleImageProcessing
{
    Q_OBJECT
public:
    ImageTrackerCentroid(QObject* parent = 0);
    ImageTrackerCentroid(int width, int height, int size ,QObject* parent = 0); 

   ~ImageTrackerCentroid();

        FilterBlotchClass FilterBlotch;

    ThresholdOptimizatorEngine ThresholdAdjuster{PROCESS_METHOD::PARALLEL_BY_DISPERSION};
    cv::Ptr<cv::BackgroundSubtractor> backSubstractor;
    bool FLAG_SUBSTRACT_BACKGROUND = false;

    void FindObjectCentroid(cv::Mat& Image);
    void TrackObjectCentroid(cv::Mat& Image, cv::Rect& ROI);
    bool ProcessImage(cv::Mat& Image);

    QPair<float,float> GetCentroid(cv::Mat& Image);
    void setInput(const QPair<float,float>& Coord) override;
    bool isIntersects(ImageTrackerCentroid& Tracker);
    bool isIntersects(std::shared_ptr<ImageTrackerCentroid> Tracker);
    bool isIntersects(const QPair<float,float>& Coord);
    bool isTrackHold() override { return StateProcessing != StatesModule::Idle; }

    //====================================================
    std::function<void (cv::Mat&, cv::Mat&)> FilterSharpen;
    std::function<void (cv::Mat&, cv::Mat&)> FilterSobel;
    std::function<void (cv::Mat&, cv::Mat&)> FilterErosion;

    std::function<void (cv::Mat&)> TrackingProcess1;
    std::function<void (cv::Mat&)> TrackingProcess2;

    std::function<void (cv::Mat&)> NodeMedianFilter;
    std::function<void (cv::Mat&)> NodeThresholdFilter;
    std::function<void (cv::Mat&)> NodeErosion;
    void makeFilters();
    //====================================================


public  slots:
   void SlotProcessImage() override;
   void SlotProcessImage(const cv::Mat& Image) override;

   void SlotResetProcessing() override;
};


#endif 
