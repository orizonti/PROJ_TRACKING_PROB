#include "widget_camera_control.h"
#include "ui_widget_camera_control.h"
WidgetCameraControl::WidgetCameraControl(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetCameraControl)
{
    ui->setupUi(this);
}

void WidgetCameraControl::LinkToDevice(std::shared_ptr<CameraControlInterface> CameraInterface)
{

  connect(ui->butSetCameraROI,&QPushButton::clicked,[this, CameraInterface]()
  {
    QStringList params = ui->lineEditROI->text().split(":");
    CameraInterface->SlotSetCameraRegion(params.at(0).toInt(), 
                                         params.at(1).toInt(), 
                                         params.at(2).toInt(),
                                         params.at(3).toInt());
  });

  connect(ui->butStartStopWork,&QPushButton::toggled,[this, CameraInterface](bool StartStop)
  {
    qDebug() << "WINDOW CONTROL START CAMERA: "  << StartStop;
    if(StartStop) CameraInterface->StartCameraStream();
    else          CameraInterface->StopCameraStream();
  });
}
