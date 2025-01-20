#ifndef IMAGE_PROCESSING_WIDGET_H
#define IMAGE_PROCESSING_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QLine>
#include "widget_adjustable.h"
#include "CV_IMAGE_PROCESSING/cv_image_processing.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetProcessingImageControl; }
QT_END_NAMESPACE


class WidgetProcessingImageControl : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetProcessingImageControl(QWidget* parent = 0);

    void LinkToModule(std::shared_ptr<CVImageProcessing> ControlInterface);

public  slots:

private:
Ui::WidgetProcessingImageControl *ui;
};

#endif // IMAGE_PROCESSING_WIDGET_H
