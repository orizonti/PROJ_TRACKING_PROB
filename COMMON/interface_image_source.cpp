#include "interface_image_source.h"
ImageSourceInterface::ImageSourceInterface(QObject* parent): QObject(parent){};


void ImageSourceInterface::adjustImageReceiver(cv::Mat& receiver)
{
   auto frame = getImageToProcess();

   if(frame.cols*frame.rows*frame.depth() != receiver.cols*receiver.rows*receiver.depth())
   receiver = frame.clone();
   
}