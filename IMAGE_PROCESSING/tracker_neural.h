#ifndef NEURAL_TRACKER_H
#define NEURAL_TRACKER_H

#include "image_processing_node.h"


class NeuralTracker : public ModuleImageProcessing
{
    Q_OBJECT
public:
    explicit NeuralTracker(QObject* parent = 0) {};
    ~NeuralTracker() {};
    QString info;

public  slots:
   void SlotProcessImage(const cv::Mat& Image) override {};
   void SlotProcessImage() override {};
   void SlotResetProcessing() override {};
  
};

#endif 
