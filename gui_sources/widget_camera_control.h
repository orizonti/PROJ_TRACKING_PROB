#ifndef CAMERA_WIDGET_H
#define CAMERA_WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QLine>
#include "widget_adjustable.h"
#include "CAMERA_INTERFACE/camera_interface_class.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WidgetCameraControl; }
QT_END_NAMESPACE


class WidgetCameraControl : public WidgetAdjustable
{
    Q_OBJECT
public:
    explicit WidgetCameraControl(QWidget* parent = 0);
    void LinkToDevice(std::shared_ptr<CameraControlInterface> CameraInterface);

private:
Ui::WidgetCameraControl *ui;

signals:
void SignalStartStream(bool);
};

#endif // CAMERA_WIDGET_H
