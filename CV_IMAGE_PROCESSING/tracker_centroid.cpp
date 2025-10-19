
#include "AIM_IMAGE_IMITATION/imitator_image_aim.h"

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


ImageTrackerCentroid::ImageTrackerCentroid(QObject* parent) : ModuleImageProcessing(parent)
{
  qRegisterMetaType<const cv::Mat&>();

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
                       cv::medianBlur(Image, Image,3); cv::medianBlur(Image, Image,3); CalcCentroid(Image); 
      //cv::threshold (ImageProcessingROI,ImageProcessingROI,10,256,cv::THRESH_BINARY);
    };

    TrackingProcess2 = [this,NodesList](cv::Mat& Image)
    { 
      Threshold = 20;
      for(auto& Node: NodesList) Node(Image);
      CalcCentroid(Image); 
    };
}

ImageTrackerCentroid::~ImageTrackerCentroid()
{
 qDebug() << TAG_NAME.c_str() << "[ DELETE ]";
}

bool ImageTrackerCentroid::CheckCentroid() 
{
  auto& sum    = ValuesImage[0]; 
  auto& width  = ValuesImage[3]; 
  auto& height = ValuesImage[4]; 
                 ValuesImage[5] = width/height;
                 ValuesImage[6] = height/width;

  qDebug() << OutputFilter::Filter(100) << "[ CHECK CENTROID ]" << "NUM:" << NumberModule 
                      << "SUM: " << sum/255 
                      << "SUM CHECK: "<<(sum < MaxImageWeight && sum > MinImageWeight);

  if(sum > MaxImageWeight || 
     sum < MinImageWeight) return false;

  //if(ValuesImage[5] > MaxImageDistortion || 
  //   ValuesImage[6] > MaxImageDistortion   )  return false;


  return true;
}

bool ImageTrackerCentroid::CalcCentroid(cv::Mat& Image)
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



void ImageTrackerCentroid::TrackObjectCentroid(cv::Mat& Image, cv::Rect& ROI)
{
             CheckCorrectROI(ROI);
  ImageProcessingROI = Image(ROI);
  //====================================================
  ProcessTimePoint = std::chrono::high_resolution_clock::now();
  CalcCentroid(ImageProcessingROI); 
  //auto Dur = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - ProcessTimePoint);
  //counter++; if(counter % 100 == 0) qDebug() << "PROCESS DURATION - " << Dur.count();
  //std::this_thread::sleep_for(std::chrono::milliseconds(2));
  //====================================================

  //TrackingProcess2(ImageProcessingROI); 

  if(FLAG_SLAVE_MOVE) FLAG_OBJECT_HOLD = CheckCentroid(); else FLAG_OBJECT_HOLD = true;

  CoordsObject[0].first  += ROI.x; 
  CoordsObject[0].second += ROI.y; if(FLAG_SLAVE_MOVE) return;
  
  ROI = cv::Rect(CoordsObject[0].first  - ROI_SIZE/2, 
                 CoordsObject[0].second - ROI_SIZE/2 ,ROI_SIZE,ROI_SIZE);



  
}

void ImageTrackerCentroid::FindObjectCentroid(cv::Mat& Image)
{
  if(ImageInputTemp.size().width != ImageInput.size().width || 
     ImageInputTemp.size().height != ImageInput.size().height)
     ImageInputTemp = ImageInput.clone();

  FilterErosion(Image, ImageInputTemp); 
  FilterSharpen(ImageInputTemp, Image); 
  // FilterErosion(ImageInputTemp, Image);

  //Image >> ThresholdAdjuster >> Threshold;

  cv::medianBlur(Image,ImageInputTemp,7);
  cv::medianBlur(Image,ImageInputTemp,7);
   cv::threshold(Image,Image,Threshold,256,cv::THRESH_BINARY);

  //FilterBlotch.FilterImage(Image, Image); 
              CalcCentroid(Image); 

                             CoordsObject[0] >> StatisticCoord; 
   RectsObject[0] = cv::Rect(CoordsObject[0].first  - Image.cols/4, 
                             CoordsObject[0].second - Image.rows/4 ,Image.cols/2,Image.rows/2);
   CheckCorrectROI(RectsObject[0]);

  if(StatisticCoord.IsCoordLoaded()) 
     StatisticCoord.GetDispersionNorm() >> Saturation(20) 
                                        >> InversionBinary 
                                        >> StatisticDispersion;

                                        if(StatisticDispersion.IsValueLoaded()) 
                                           StatisticDispersion.GetAvarageValue() >> TrackingDetector(0.9);

   FLAG_TRACK_MODE = TrackingDetector.isSignal(); if(FLAG_TRACK_MODE) Threshold *= 1.1;
                                                  if(FLAG_TRACK_MODE) SourceImage->skipFrames();
                                                  if(FLAG_TRACK_MODE) this->SetHighFrequencyProcessing();
   //qDebug() << Filter3 << "THRESHOLD"
}

void ImageTrackerCentroid::SlotProcessImage()
{
          if(  SourceImage == nullptr || 
              !SourceImage->isFrameAvailable()) return;
                                                                    FrameMeasureInput++;
  MutexImageAccess.lock();
  ImageInput = SourceImage->getImageToProcess().clone(); 
            if(SourceImage->getAvailableFrames() > 2) SourceImage->skipFrames();
  ImageProcessing = ImageInput;

                                                                    FrameMeasureProcess++; 
  if(FLAG_SLAVE_MOVE || FLAG_TRACK_MODE) 
  TrackObjectCentroid(ImageProcessing, RectsObject[0]);
  else                  
  FindObjectCentroid(ImageProcessing);  
                                           FrameMeasureProcess++;
  //qDebug() << OutputFilter::Filter(50) << CoordsObject[0].first 
  //                                     << CoordsObject[0].second << FrameMeasureProcess.printPeriod() << info;

  MutexImageAccess.unlock();

  emit ImageSourceInterface::signalNewImage();

  if(FLAG_OBJECT_HOLD ) passCoord(); 


}


void ImageTrackerCentroid::SlotResetProcessing()
{
   ModuleImageProcessing::SlotResetProcessing(); 
   qDebug() << TAG_NAME << "[ NUMBER ]" << NumberModule << "[ CENTROID RESET]";

   TrackHoldDetector.Reset();
}

void ImageTrackerCentroid::CalcThreshold() 
{
  //if(OptimizationLumin.isEnabled())
  //{
  //ImageProcessing >> OptimizationLumin >> Threshold;
  //                                        Threshold >> StatisticThreshold;
  //                   OptimizationLumin.GetValue() >> OptimizationDisp;

  //if(StatisticThreshold.GetDispersionValue() < 1) OptimizationLumin.Disable();
  //return;
  //}

  //ImageProcessing >> OptimizationDisp >> Threshold;
}


                                      
//==============================================================================================
//TRACKER GPU
ImageTrackerCentroidGPU::ImageTrackerCentroidGPU(QObject* parent) : ImageTrackerCentroid(parent)
{
  qRegisterMetaType<const cv::Mat&>();
  info = "GPU";
    //==================================================================
    //
    cv::Mat TempMat = (cv::Mat_<double>(3,3) << 0, 0, 0, 
                                                0, 1, 0, 
                                                0, 0, 0);

            cv::UMat kernel1(3,3,CV_8UC1); TempMat.copyTo(kernel1); 
    FilterSharpenGPU = [kernel1,this](cv::UMat& Image, cv::UMat& ImageOutput)
    { 
    filter2D(Image, ImageOutput, -1 , kernel1, cv::Point(-1, -1), 0, 4);
    };

    //====================================================================
   TempMat = (cv::Mat_<double>(3,3) << -1, 0, 1, 
                                       -2, 0, -2, 
                                       -1, 0, 1);
             cv::UMat kernel2(3,3,CV_8UC1); 
                      kernel2 = TempMat.getUMat(cv::ACCESS_RW);

    FilterSobelGPU = [kernel2,this](cv::UMat& Image, cv::UMat& ImageOutput)
    { 
    filter2D(Image, ImageOutput, -1 , kernel2, cv::Point(-1, -1), 0, 4);
    };
    //====================================================================
    
    int erosion_size = 2;
    int dilation_size = 2;
    //int erosion_type = cv::MORPH_RECT;
    int erosion_type = cv::MORPH_CROSS;
    int dilation_type = cv::MORPH_RECT;

    TempMat = getStructuringElement( erosion_type,
                      cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                      cv::Point( erosion_size, erosion_size ) );

    cv::UMat ErosionMat = TempMat.getUMat(cv::ACCESS_RW);

    ImageGPU.create(400,400,CV_8UC1); 
    ImageGPUTemp.create(400,400,CV_8UC1);
    ImageGPU_ROI.create(80,80,CV_8UC1);
    ImageGPU_ROI2.create(80,80,CV_8UC1);

    FilterErosionGPU = [ErosionMat,this](cv::UMat& Image, cv::UMat& ImageOutput)
    { 
    erode( Image, ImageOutput, ErosionMat );
    };

    //====================================================================

}

void ImageTrackerCentroidGPU::FindObjectCentroidGPU(cv::Mat& Image)
{
       info = "GPU";

       Image.copyTo(ImageGPU);
    if(ImageGPU.size() != Image.size())
    {
       ImageGPUTemp.reshape(Image.cols, Image.rows);
           ImageGPU.reshape(Image.cols, Image.rows);
    }

    //FilterErosionGPU(ImageGPUTemp, ImageGPU); 
    //FilterSharpenGPU(ImageGPU, ImageGPUTemp); 
    //FilterErosionGPU(ImageGPUTemp, ImageGPU);
    //FilterErosionGPU(ImageGPUTemp, ImageGPU); 
   cv::medianBlur(ImageGPU,ImageGPU,7);
   cv::medianBlur(ImageGPU,ImageGPU,7);
   cv::medianBlur(ImageGPU,ImageGPU,7);
   cv::medianBlur(ImageGPU,ImageGPU,7);

    cv::threshold(ImageGPU,ImageGPU,Threshold,256,cv::THRESH_BINARY);

    //Image = ImageGPU.getMat(cv::ACCESS_READ);
    //Image >> ThresholdAdjuster >> Threshold;
    //FilterBlotch.FilterImage(Image, Image); 

    CalcCentroid(ImageGPU); 

    RectsObject[0] = cv::Rect(CoordsObject[0].first  - Image.cols/4, 
                              CoordsObject[0].second - Image.rows/4 ,Image.cols/2,Image.rows/2);
    CheckCorrectROI(RectsObject[0]);

    CoordsObject[0] >> TrackHoldDetector(0.9,20);

       FLAG_TRACK_MODE = TrackHoldDetector.isTrackHold(); 
    if(FLAG_TRACK_MODE) Threshold *= 1.1;
    if(FLAG_TRACK_MODE) SourceImage->skipFrames();
    if(FLAG_TRACK_MODE) this->SetHighFrequencyProcessing();

}

void ImageTrackerCentroidGPU::TrackObjectCentroidGPU(cv::Mat& Image, cv::Rect& ROI)
{
             CheckCorrectROI(ROI);
  Image(ROI).copyTo(ImageGPU_ROI);
   qDebug() << OutputFilter::Filter(30) << "TRACK GPU";

   cv::medianBlur(ImageGPU_ROI,ImageGPU_ROI,7);
   cv::medianBlur(ImageGPU_ROI,ImageGPU_ROI,7);
   cv::medianBlur(ImageGPU_ROI,ImageGPU_ROI,7);
   cv::medianBlur(ImageGPU_ROI,ImageGPU_ROI,7);

   cv::threshold(ImageGPU_ROI ,ImageGPU_ROI,10,256,cv::THRESH_BINARY);
    CalcCentroid(ImageGPU_ROI); 

    CoordsObject[0].first  += ROI.x; 
    CoordsObject[0].second += ROI.y;

  if(FLAG_SLAVE_MOVE) FLAG_OBJECT_HOLD = CheckCentroid(); else FLAG_OBJECT_HOLD = true;
  if(FLAG_SLAVE_MOVE) return;
  
  ROI = cv::Rect(CoordsObject[0].first  - ROI_SIZE/2, 
                 CoordsObject[0].second - ROI_SIZE/2 ,ROI_SIZE,ROI_SIZE);


}

bool ImageTrackerCentroidGPU::CalcCentroid(cv::UMat& Image)
{
    auto moments = cv::moments(Image);
    
    auto cx = moments.m10/moments.m00;
    auto cy = moments.m01/moments.m00;

    CoordsObject[0].first  = cx; 
    CoordsObject[0].second = cy;

    return true;
}

void ImageTrackerCentroidGPU::SlotProcessImage()
{
                             if( SourceImage == nullptr) return;
                             if(!SourceImage->isFrameAvailable()) return;
                             FrameMeasureInput++;
  MutexImageAccess.lock();
                    ImageInput = SourceImage->getImageToProcess().clone(); 
                              if(SourceImage->getAvailableFrames() > 2) SourceImage->skipFrames();
  MutexImageAccess.unlock();
  ImageProcessing = ImageInput;

                                           FrameMeasureProcess++; 
  if(FLAG_SLAVE_MOVE || FLAG_TRACK_MODE) 
  TrackObjectCentroidGPU(ImageProcessing, RectsObject[0]);
  else                  
  FindObjectCentroidGPU(ImageProcessing);  
                                           FrameMeasureProcess++;
  qDebug() << OutputFilter::Filter(30) << FrameMeasureProcess.printPeriod() << info;


  emit ImageSourceInterface::signalNewImage();

  if(FLAG_OBJECT_HOLD ) passCoord(); 

}

ImageTrackerCentroidGPU::~ImageTrackerCentroidGPU()
{
 qDebug() << TAG_NAME.c_str() << "[ DELETE ]" << info;
}

//==============================================================================================
  //auto Dur = std::chrono::duration_cast<std::chrono::microseconds>( ProcessTimePoint2 - ProcessTimePoint);
                                      //std::this_thread::sleep_for(std::chrono::microseconds(100));
                                      //
                                      //
//  FilterBlotch.FilterImage(ImageProcessingROI, ImageProcessingROI); 
//  ProcessTimePoint = std::chrono::high_resolution_clock::now();

//  Threshold = 20;
//  FilterErosion(ImageProcessingROI, ImageProcessingROI); 
//  FilterErosion(ImageProcessingROI, ImageProcessingROI); 
//  cv::medianBlur(ImageProcessingROI, ImageProcessingROI,3);
//  cv::medianBlur(ImageProcessingROI, ImageProcessingROI,3);
//  cv::threshold (ImageProcessingROI,ImageProcessingROI,10,256,cv::THRESH_BINARY);
//  CalcCentroid(ImageProcessingROI); 

//  std::this_thread::sleep_for(std::chrono::milliseconds(2));
//  ProcessTimePoint = std::chrono::high_resolution_clock::now();
