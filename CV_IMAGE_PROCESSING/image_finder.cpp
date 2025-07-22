#include "image_finder.h"

ImageTemplateFinder::ImageTemplateFinder(QObject* parent) : ModuleImageProcessing(parent)
{
             if(QFile::exists("/home/orangepi/DATA/UFO.png"))
   ImageTemplate = cv::imread("/home/orangepi/DATA/UFO.png",cv::IMREAD_GRAYSCALE );
             else qDebug() << "[ IMAGE TEMPLATE FINDER ]" << "/home/orangepi/DATA/UFO.png" << "[NOT EXISTS]";

             if(QFile::exists("/home/broms/DATA/UFO.png"))
   ImageTemplate = cv::imread("/home/broms/DATA/UFO.png",cv::IMREAD_GRAYSCALE );
             else qDebug() << "[ IMAGE TEMPLATE FINDER ]" << "/home/broms/DATA/UFO.png" << "[NOT EXISTS]";
}

ImageTemplateFinder::~ImageTemplateFinder()
{
 qDebug() << "DELETE IMAGE TRACKER TEMPLATE";
}

void ImageTemplateFinder::FindImageTemplate(cv::Mat& Image)
{
  if(ImageTemplate.empty()) {qDebug() << TAG_NAME << " [ TRACKING TEMPLATE EMPTY ]"; return; }

                     //ImageProcessing = Image(RectsObject[0]); 
  //=================================================================================
  int result_cols = Image.cols - ImageTemplate.cols + 1;
  int result_rows = Image.rows - ImageTemplate.rows + 1;

                       ProcessResult.create( result_rows, result_cols, CV_32FC1 );
  cv::matchTemplate( ImageProcessing, ImageTemplate, 
                                      ProcessResult, cv::TM_SQDIFF); 
        cv::normalize( ProcessResult, ProcessResult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

                                double minVal; cv::Point minLoc;
                                double maxVal; cv::Point maxLoc;
        cv::minMaxLoc( ProcessResult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

  CoordsObject[0].first  = minLoc.x; 
  CoordsObject[0].second = minLoc.y; 
  //=================================================================================

  RectsObject[0] = cv::Rect(CoordsObject[0].first , 
                            CoordsObject[0].second, 80, 80);

  CheckCorrectROI(RectsObject[0]);
}


void ImageTemplateFinder::SlotProcessImage()
{
    MutexImageAccess.lock();
                 SourceImage->SwitchToNextFrame();
    ImageInput = SourceImage->GetImageToProcess().clone(); ImageProcessing = ImageInput;
    MutexImageAccess.unlock();

    FilterBlotch.FilterImage(ImageProcessing, ImageProcessing); 

      FindImageTemplate(ImageProcessing); 

}

void ImageTemplateFinder::SlotProcessImage(const cv::Mat& Image) 
{
    ImageInput = Image.clone(); ImageProcessing = ImageInput;

    FilterBlotch.FilterImage(ImageProcessing, ImageProcessing); 

      FindImageTemplate(ImageProcessing); 

}
