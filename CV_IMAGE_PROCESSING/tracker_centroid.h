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
    explicit ImageTrackerCentroid(QObject* parent = 0);
            ~ImageTrackerCentroid();
    QString info = "[ CENTROID CPU ]";

        FilterBlotchClass FilterBlotch;
    ThresholdOptimizatorEngine ThresholdAdjuster{PROCESS_METHOD::PARALLEL_BY_DISPERSION};
    
    StatisticNode<double> StatisticCoord{100};
    StatisticNode<double> StatisticDispersion{100};
    StatisticNode<double> StatisticThreshold{100};

    ValueDetector<double> TrackingDetector;
    ValueSaturation<double> Saturation;
    ValueBinaryInversion<double> InversionBinary;

    TrackHoldDetectorNode TrackHoldDetector{0.9, 10};

    cv::Mat Image;
    cv::Mat ImageTemp;

    bool CheckCentroid(); 

    void FindObjectCentroid(cv::Mat& Image);
    void TrackObjectCentroid(cv::Mat& Image, cv::Rect& ROI);

    bool CalcCentroid(cv::Mat& Image);
    void CalcThreshold() override;

    QPair<float,float> GetCentroid(cv::Mat& Image);

    std::function<void (cv::Mat&, cv::Mat&)> FilterSharpen;
    std::function<void (cv::Mat&, cv::Mat&)> FilterSobel;
    std::function<void (cv::Mat&, cv::Mat&)> FilterErosion;

    std::function<void (cv::Mat&)> TrackingProcess1;
    std::function<void (cv::Mat&)> TrackingProcess2;

    std::function<void (cv::Mat&)> NodeMedianFilter;
    std::function<void (cv::Mat&)> NodeThresholdFilter;
    std::function<void (cv::Mat&)> NodeErosion;


public  slots:
   void SlotProcessImage() override;
   void SlotProcessImage(const cv::Mat& Image) override {};

   void SlotResetProcessing() override;
};


class ImageTrackerCentroidGPU : public ImageTrackerCentroid
{
    Q_OBJECT
public:
    explicit ImageTrackerCentroidGPU(QObject* parent = 0);
            ~ImageTrackerCentroidGPU();
    QString info = "CPU";

    cv::UMat ImageGPU;
    cv::UMat ImageGPU2;
    cv::UMat ImageGPU_ROI;
    cv::UMat ImageGPU_ROI2;

    void FindObjectCentroidGPU (cv::Mat& Image);
    void TrackObjectCentroidGPU(cv::Mat& Image, cv::Rect& ROI);

    bool CalcCentroid(cv::UMat& Image);

    std::function<void (cv::UMat&, cv::UMat&)> FilterSharpenGPU;
    std::function<void (cv::UMat&, cv::UMat&)> FilterSobelGPU;
    std::function<void (cv::UMat&, cv::UMat&)> FilterErosionGPU;

public slots:
   void SlotProcessImage() override;
   void SlotProcessImage(const cv::Mat& Image) override {};
};


#endif 
