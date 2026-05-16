
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

#include "tracker_centroid.h"

#include "debug_output_filter.h"
#include "state_block_enum.h"
#include "register_settings.h"


ImageTrackerCentroid::ImageTrackerCentroid(QObject* parent) : ModuleImageProcessing("[TRACKER_CENTROID]" ,parent)
{
  makeFilters();
  SizeROI = SettingsRegister::GetValue("PROCESSING_ROI2");

  backSubstractor = cv::createBackgroundSubtractorMOG2();
}

ImageTrackerCentroid::ImageTrackerCentroid(int width, int height, int size ,QObject* parent): 
                      ModuleImageProcessing(width, height, size, "[TRACKER CENTROID]") 
{ 
  makeFilters();
  SizeROI = SettingsRegister::GetValue("PROCESSING_ROI2");
  backSubstractor = cv::createBackgroundSubtractorMOG2();
};

ImageTrackerCentroid::~ImageTrackerCentroid() { qDebug() << TAG_NAME << "[ DELETE ]"; }



bool ImageTrackerCentroid::isIntersects(const QPair<float,float>& Coord)
{
               auto Diff = Coord - CoordsObject[0];
  return std::hypot(Diff.first,Diff.second) < SizeROI*0.9;
}

bool ImageTrackerCentroid::isIntersects(ImageTrackerCentroid& Tracker)
{ return isIntersects(Tracker.getOutput());  }
bool ImageTrackerCentroid::isIntersects(std::shared_ptr<ImageTrackerCentroid> Tracker)
{ return isIntersects(Tracker->getOutput()); }


QPair<float,float> ImageTrackerCentroid::GetCentroid(cv::Mat& Image)
{
		double sum_x = 0.0, sum_y = 0.0, sum = 0.0;

		for (int row = 0; row < Image.rows; row++)
		{
			uint8_t* RowData = Image.ptr<uint8_t>(row);
			for (int col = 0; col < Image.cols; col++)
			{
					sum += RowData[col];
					sum_x += RowData[col] * col;
					sum_y += RowData[col] * row;
			}
		}

    return QPair<float,float>(sum_x/sum,sum_y/sum);
}


void ImageTrackerCentroid::setInput(const QPair<float,float>& Coord)
{
  if(StateProcessing == StatesModule::Idle) SetStateActive();
  if(StateProcessing == StatesModule::WorkTrack && ModeProcessing == ModesModule::Master) return;

  MutexInput.lock();
  RectsObject[0] = cv::Rect(Coord.first  - SizeROI/2, 
                            Coord.second - SizeROI/2 , SizeROI, SizeROI);
  StateProcessing = StatesModule::WorkTrack;  
  MutexInput.unlock();
};

void ImageTrackerCentroid::SlotProcessImage()
{

                             if(SourceImage->empty()) return;
                             if(SourceImage->getAvailableFrames() > 2) SourceImage->skipFrames();

                                                     FrameMeasureInput++;   
                                                     FrameMeasureProcess++; 
       //====================================================================
                  *ImageInput = SourceImage->getImageToProcess().clone(); if((*ImageInput).empty()) return; ;  

                                          std::lock_guard<std::mutex> locker1(MutexInput);
                                          std::lock_guard<std::mutex> locker2(MutexImageAccess);
       //====================================================================
                      ImageProcessing = *ImageInput;
       TrackObjectCentroid(ImageProcessing, RectsObject[0]); 
                            ImageInput++; if(ImageInput == ImagesInput.end()) 
                                             ImageInput = ImagesInput.begin();
       //====================================================================
                                                     FrameMeasureProcess++;

       if(isTrackHold()) PassCoordClass<float>::passCoord(); 

       MutexImageAccessDisplay.lock(); ImageOutput = *ImageInput; MutexImageAccessDisplay.unlock();
       //if( isTrackHold()) emit ModuleImageProcessing::signalCoord(CoordsObject[0]); 
       //qDebug() << OutputFilter::Filter(50) << "[PROCESS TIME]" << FrameMeasureProcess.getMilliseconds();
}

void ImageTrackerCentroid::SlotProcessImage(const cv::Mat& Image) 
{

                                            FrameMeasureInput++;
                                            FrameMeasureProcess++; MutexImageAccess.lock();

                           if(Image.empty()) return;  
                *ImageInput = Image.clone(); 

  TrackObjectCentroid(*ImageInput, RectsObject[0]); 
                       ImageInput++; if(ImageInput == ImagesInput.end()) 
                                        ImageInput = ImagesInput.begin();

                                            FrameMeasureProcess++; MutexImageAccess.unlock();

     if(isTrackHold()) PassCoordClass<float>::passCoord(); 
     MutexImageAccessDisplay.lock(); ImageOutput = *ImageInput; MutexImageAccessDisplay.unlock();
}

void ImageTrackerCentroid::TrackObjectCentroid(cv::Mat& Image, cv::Rect& ROI)
{
  if(StateProcessing == StatesModule::Idle) return;
             CheckCorrectROI(ROI);

                             ImageProcessingROI = Image(ROI); if(ImageProcessingROI.empty()) return;
    //FilterBlotch.FilterImage(ImageProcessingROI);
    //
         cv::minMaxLoc(ImageProcessingROI, &MinPixel, &MaxPixel);
		 Threshold = MinPixel + (MaxPixel - MinPixel)*0.2;

  //FilterErosion(ImageProcessingROI, ImageProcessingROI); 
  //cv::morphologyEx(ImageProcessingROI, ImageProcessingROI, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5))); 
  //====================================================
  bool isNoSignal =  ProcessImage(ImageProcessingROI); if(!isNoSignal) return;

  CoordsObject[0].first  += ROI.x; 
  CoordsObject[0].second += ROI.y; if(ModeProcessing == ModesModule::Slave) return;
  
  ROI = cv::Rect(CoordsObject[0].first  - SizeROI/2, 
                 CoordsObject[0].second - SizeROI/2 ,SizeROI,SizeROI);
  
}

bool ImageTrackerCentroid::ProcessImage(cv::Mat& Image)
{
    auto& sum   = ValuesImage[0]; sum   = 0;
    auto& sum_x = ValuesImage[1]; sum_x = 0; 
    auto& sum_y = ValuesImage[2]; sum_y = 0; uint8_t* RowData = nullptr;

    int sum_width  = 0; ValuesImage[3] = sum_width;
    int sum_height = 0; ValuesImage[4] = sum_height;

		for (int row = 0; row < Image.rows; row++)
		{
          RowData = Image.ptr<uint8_t>(row);
          sum_width = 0;
			for (int col = 0; col < Image.cols; col++)
			{
				  if (RowData[col] < Threshold) { RowData[col] = 0; continue; } RowData[col] = 255;

					sum   += RowData[col];
					sum_x += RowData[col] * col;
					sum_y += RowData[col] * row;

             sum_width++;
			}
          if(sum_width > 5) sum_height++;
          if(sum_width > ValuesImage[3]) ValuesImage[3] = sum_width;
		}
                                         ValuesImage[4] = sum_height;

		if (sum == 0) return false;

    CoordsObject[0].first  = sum_x / sum; 
    CoordsObject[0].second = sum_y / sum;
    return true;
}


void ImageTrackerCentroid::SlotResetProcessing() { ModuleImageProcessing::SlotResetProcessing(); }

void ImageTrackerCentroid::makeFilters()
{
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
    NodeThresholdFilter = [this](cv::Mat& Image) { cv::threshold (Image,Image,10,256,cv::THRESH_BINARY); };

    std::vector<std::function<void (cv::Mat&)>> NodesList;
    NodesList.push_back(NodeErosion);
    NodesList.push_back(NodeMedianFilter);
    NodesList.push_back(NodeMedianFilter);


    TrackingProcess1 = [this](cv::Mat& Image)
    { 
       Threshold = 20; FilterErosion(Image, Image); FilterErosion(Image, Image); 
                       cv::medianBlur(Image, Image,3); cv::medianBlur(Image, Image,3); ProcessImage(Image); 
      //cv::threshold (ImageProcessingROI,ImageProcessingROI,10,256,cv::THRESH_BINARY);
    };

    TrackingProcess2 = [this,NodesList](cv::Mat& Image)
    { 
      Threshold = 20;
      for(auto& Node: NodesList) Node(Image);
      ProcessImage(Image); 
    };
}
                                      


