#ifndef IMAGE_FINDER_H
#define IMAGE_FINDER_H
#include "image_processing_node.h"
#include "contour_processing.h"
#include "filter_blotch.h"


class ImageTemplateFinder : public ModuleImageProcessing
{
    Q_OBJECT
public:
    explicit ImageTemplateFinder(QObject* parent = 0);
    ~ImageTemplateFinder();

    ContoursProcessorClass ContoursProcessor;
        FilterBlotchClass  FilterBlotch;

    std::vector<cv::Mat> StoreTemplates;

    cv::Mat  ImageTemplate;
    cv::Mat  ProcessResult;
    cv::Size TemplateRect;

    void FindImageTemplate(cv::Mat& Image);

    void CalcObjectTrajectory();

public  slots:
   void SlotProcessImage(const cv::Mat& Image) override;
   void SlotProcessImage() override;
   void SlotResetProcessing() override {};
};
#endif 
