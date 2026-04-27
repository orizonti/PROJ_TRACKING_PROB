#include "interface_image_source.h"

void SourceImageDisplayInterface::passToLink() { if(LinkNode != nullptr) LinkNode->setImageFrom(this); }

void SourceImageInterface::adjustImageReceiver(cv::Mat& receiver)
{
  auto frame = getImageToProcess();
  if(frame.cols*frame.rows*frame.depth() != receiver.cols*receiver.rows*receiver.depth())
    receiver = frame.clone();
};
