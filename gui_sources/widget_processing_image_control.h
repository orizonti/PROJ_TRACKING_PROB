#ifndef IMAGE_PROCESSING_WIDGET_H
#define IMAGE_PROCESSING_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QLine>
#include "widget_adjustable.h"
#include "IMAGE_PROCESSING/image_processing_node.h"
#include "CAMERA_INTERFACE/camera_control_interface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetProcessingImageControl; }
QT_END_NAMESPACE


class WidgetProcessingImageControl : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetProcessingImageControl(QWidget* parent = 0);

    void linkToModule(std::shared_ptr<ModuleImageProcessing> ControlInterface);
    void linkToModule(CameraControlInterface* ControlInterface);
    void HideLabel();
    void setParamRange(int Range, int Steps);
signals:
void SignalResetProcessing();

private:
Ui::WidgetProcessingImageControl *ui;
};

#endif // IMAGE_PROCESSING_WIDGET_H
