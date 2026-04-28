#include "image_finder.h"
#include "register_settings.h"

ImageTemplateFinder::ImageTemplateFinder(QObject* parent) : ModuleImageProcessing("[TEMPLATE FINDER]",parent)
{
}

ImageTemplateFinder::~ImageTemplateFinder() { }

void ImageTemplateFinder::FindImageTemplate(cv::Mat& Image)
{
  if(ImageTemplate.empty()) {qDebug() << TAG_NAME << " [ TRACKING TEMPLATE EMPTY ]"; return; }

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
                 SourceImage->switchToNextFrame();
    *ImageInput = SourceImage->getImageToProcess().clone(); ImageProcessing = *ImageInput;
    MutexImageAccess.unlock();

    FilterBlotch.FilterImage(ImageProcessing); 

      FindImageTemplate(ImageProcessing); 

}

void ImageTemplateFinder::SlotProcessImage(const cv::Mat& Image) 
{
    *ImageInput = Image.clone(); ImageProcessing = *ImageInput;

    FilterBlotch.FilterImage(ImageProcessing); 
                 FindImageTemplate(ImageProcessing); 

}
